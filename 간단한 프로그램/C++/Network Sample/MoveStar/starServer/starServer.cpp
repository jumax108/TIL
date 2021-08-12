#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <new>

#include "stack.h"
#include "ringBuffer.h"

#pragma comment(lib, "ws2_32")

const WCHAR* SERVER_IP = L"0.0.0.0";
constexpr USHORT SERVER_PORT = 3000;

SOCKET listenSocket;

constexpr int SEND_BUF_SIZE = 3000;
constexpr int RECV_BUF_SIZE = 3000;


struct stPlayer {

	UINT id;
	SOCKET sock;
	UINT x;
	UINT y;

	CRingBuffer sendBuf;
	CRingBuffer recvBuf;

};


UINT baseId = 1;

constexpr BYTE MAX_PLAYER_NUM = 63;
CStack<BYTE> playersIndexStack(63);
stPlayer players[63];

enum class MESSAGE_TYPE {
	SET_ID = 0,
	CREATE_STAR,
	REMOVE_STAR,
	MOVE_STAR
};

struct stMsgHeader {
	UINT type;
	UINT id;
};

struct stMsgSetID {
	stMsgHeader head;
	BYTE unUsed[8];
};

struct stMsgCreateStar {
	stMsgHeader head;
	UINT x;
	UINT y;
};

struct stMsgRemoveStar {
	stMsgHeader head;
	BYTE unUsed[8];
};

struct stMsgMoveStar{
	stMsgHeader head;
	UINT x;
	UINT y;
};

void sendUnicast(stPlayer* player, BYTE* msg, UINT msgSize);
void sendBroadcast(stPlayer* exceptplayer, BYTE* msg, UINT msgSize);

bool networkInit() {

	WSADATA wsa;
	int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsa);
	int wsaStartupError;
	if (wsaStartupResult != NULL) {
		wsaStartupError = WSAGetLastError();
		wprintf(L"wsa startup error : %d\n", wsaStartupError);
		return false;
	}

	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	int listenSocketError;
	if (listenSocket == INVALID_SOCKET) {
		listenSocketError = WSAGetLastError();
		wprintf(L"listen socket error : %d\n", listenSocketError);
		return false;
	}

	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	InetPtonW(AF_INET, SERVER_IP, &serverAddr.sin_addr.s_addr);
	serverAddr.sin_port = htons(SERVER_PORT);
	int bindResult = bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR_IN));
	int bindError;
	if (bindResult == SOCKET_ERROR) {
		bindError = WSAGetLastError();
		wprintf(L"bind error : %d\n", bindError);
		return false;
	}

	int listenResult = listen(listenSocket, SOMAXCONN);
	int listenError;
	if (listenResult == SOCKET_ERROR) {
		listenError = WSAGetLastError();
		wprintf(L"listen error : %d\n", listenError);
		return false;
	}

	u_long on = 1;
	int ioctlResult = ioctlsocket(listenSocket, FIONBIO, &on);
	int ioctlError;
	if (ioctlResult == SOCKET_ERROR) {
		ioctlError = WSAGetLastError();
		wprintf(L"ioctl error : %d\n", ioctlError);
		return false;
	}

	//int 

	return true;

}

bool init() {

	bool stackPushResult;
	for (BYTE playerCnt = 0; playerCnt < MAX_PLAYER_NUM; ++playerCnt) {
		stackPushResult = playersIndexStack.push(playerCnt);
		if (stackPushResult == false) {
			wprintf(L"stack push error \n file: %s, line: %d, playerCnt: %d\n", __FILEW__, __LINE__, playerCnt);
			return false;
		}
	}

	ZeroMemory(players, MAX_PLAYER_NUM * sizeof(stPlayer));

	return true;

}

bool acceptConnect() {

	BYTE playerIndex;
	bool frontResult;

	frontResult = playersIndexStack.front(&playerIndex);
	playersIndexStack.pop();

	stPlayer* player = &players[playerIndex];

	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	player->sock = accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);

	int socketError;
	if (player->sock == INVALID_SOCKET) {

		socketError = WSAGetLastError();
		wprintf(L"accept error : %d\n", socketError);
		return false;
	}

	player->x = 5;
	player->y = 5;

	player->id = baseId;
	baseId += 1;

	new (&player->recvBuf) CRingBuffer(RECV_BUF_SIZE);
	new (&player->sendBuf) CRingBuffer(SEND_BUF_SIZE);

	stMsgSetID msgSetId;
	msgSetId.head.id = player->id;
	msgSetId.head.type = (UINT)MESSAGE_TYPE::SET_ID;
	sendUnicast(player, (BYTE*)&msgSetId, sizeof(stMsgSetID));

	stMsgCreateStar msgCreateStar;
	msgCreateStar.head.id = player->id;
	msgCreateStar.head.type = (UINT)MESSAGE_TYPE::CREATE_STAR;
	msgCreateStar.x = player->x;
	msgCreateStar.y = player->y;
	sendBroadcast(player, (BYTE*)&msgCreateStar, sizeof(stMsgCreateStar));

	stPlayer* playerIter;
	stPlayer* playerStart = players;
	stPlayer* playerEnd = players + MAX_PLAYER_NUM;

	for (playerIter = playerStart; playerIter != playerEnd; ++playerIter) {

		if (playerIter->id == 0) {
			continue;
		}

		msgCreateStar.head.id = playerIter->id;
		msgCreateStar.x = playerIter->x;
		msgCreateStar.y = playerIter->y;
		sendUnicast(player, (BYTE*)&msgCreateStar, sizeof(stMsgCreateStar));

	}

	WCHAR ip[20] = {0,};
	InetNtopW(AF_INET, &clientAddr.sin_addr.s_addr, ip, 20);
	USHORT port = ntohs(clientAddr.sin_port);
	wprintf(L"Connect Client, ip: %s, port: %d, id: %d\n", ip, port, player->id);


	return true;
}

void disconnect(stPlayer* player) {

	if (player->id == 0) {
		return;
	}

	BYTE playerIndex = 0;

	stPlayer* playerIter;
	stPlayer* playerStart = players;
	stPlayer* playerEnd = players + MAX_PLAYER_NUM;

	for (playerIter = playerStart; playerIter != playerEnd; ++playerIter, playerIndex++) {

		if (playerIter == player) {
			break;
		}

	}

	stMsgRemoveStar msg;
	msg.head.id = player->id;
	msg.head.type = (UINT)MESSAGE_TYPE::REMOVE_STAR;
	sendBroadcast(nullptr, (BYTE*)&msg, sizeof(msg));

	wprintf(L"Disconnect Client, id: %d\n", player->id);

	player->id = 0;

	player->recvBuf.~CRingBuffer();
	player->sendBuf.~CRingBuffer();

	playersIndexStack.push(playerIndex);

	closesocket(player->sock);
}

void sendUnicast(stPlayer* player, BYTE* msg, UINT msgSize) {

	player->sendBuf.push(msgSize, msg);
	wprintf(L"UniCast, send to(id): %d, type: %d, from(id): %d\n", player->id, *(int*)msg, *(((int*)msg)+1));

}

void sendBroadcast(stPlayer* exceptPlayer, BYTE* msg, UINT msgSize) {

	stPlayer* playerIter;
	stPlayer* playerStart = players;
	stPlayer* playerEnd = players + MAX_PLAYER_NUM;

	for (playerIter = playerStart; playerIter != playerEnd; ++playerIter){

		if (playerIter == exceptPlayer || playerIter->id == 0) {
			continue;
		}

		playerIter->sendBuf.push(msgSize, msg);
		wprintf(L"BroadCast, send to(id): %d, type: %d, from(id): %d\n", playerIter->id, *(int*)msg, *(((int*)msg) + 1));

	}

}

void packetProcess(stPlayer* player) {
	printf("%d\n", player->recvBuf.size());
	while (player->recvBuf.size() >= sizeof(stMsgHeader)) {

		stMsgHeader head;
		bool frontResult;
		CRingBuffer* playerRecvBuf = &player->recvBuf;
		frontResult = playerRecvBuf->front(sizeof(stMsgHeader), (BYTE*)&head);
		playerRecvBuf->pop(sizeof(stMsgHeader));

		// 현재 메시지 내용이 무조건 8바이트임
		constexpr int msgBodySize = 8;
		BYTE msgBody[msgBodySize];
		int playerRecvBufSize = playerRecvBuf->size();
		int leftRecvBufSize = RECV_BUF_SIZE - playerRecvBufSize;

		if (leftRecvBufSize < msgBodySize) {
			playerRecvBuf->front(leftRecvBufSize, msgBody);
			playerRecvBuf->pop(leftRecvBufSize);
			playerRecvBuf->push(sizeof(stMsgHeader), (BYTE*)&head);
			playerRecvBuf->push(leftRecvBufSize, msgBody);
			break;
		}

		switch ((MESSAGE_TYPE)head.type) {
		case MESSAGE_TYPE::MOVE_STAR: 
			{
				stMsgMoveStar msg;

				playerRecvBuf->front(msgBodySize, msgBody);
				playerRecvBuf->pop(msgBodySize);

				memcpy(&msg, &head, sizeof(head));
				memcpy((BYTE*)&msg + sizeof(head), msgBody, msgBodySize);

				wprintf(L"MoveStar, id: %d\n(%d, %d) -> (%d, %d)\n", player->id, player->x, player->y, msg.x, msg.y);

				player->x = msg.x;
				player->y = msg.y;

				sendBroadcast(player, (BYTE*)&msg, sizeof(msg));
				
			}
			break;
		default:
			disconnect(player);
			break;
		}

	}

}

bool networkRecv() {

	FD_SET readSet;
	FD_ZERO(&readSet);
	FD_SET(listenSocket, &readSet);

	FD_SET exceptSet;
	FD_ZERO(&exceptSet);
	FD_SET(listenSocket, &exceptSet);

	stPlayer* player;
	stPlayer* playerStart = players;
	stPlayer* playerEnd = playerStart + MAX_PLAYER_NUM;

	for (player = playerStart; player != playerEnd; ++player) {
		if (player->id == 0) {
			continue;
		}
		FD_SET(player->sock, &readSet);
	}

	timeval delayTime;
	delayTime.tv_sec = 0;
	delayTime.tv_usec = 0;
	int selectResult;
	selectResult = select(0, &readSet, NULL, &exceptSet, &delayTime);
	if (selectResult > 0) {
		
		if (FD_ISSET(listenSocket, &exceptSet) != 0) {

			wprintf(L"listen socket connect except\n");

		} else if (FD_ISSET(listenSocket, &readSet) != 0) {
			bool connectResult;
			connectResult = acceptConnect();
			if (connectResult == false) {
				return false;
			}
		}

		int bufSize;
		char buf[RECV_BUF_SIZE];

		for (player = playerStart; player != playerEnd; ++player) {
			SOCKET playerSocket = player->sock;
			if (FD_ISSET(playerSocket, &readSet) == 0) {
				continue;
			}

			int playerRecvBufSize = player->recvBuf.size();
			bufSize = RECV_BUF_SIZE - playerRecvBufSize;

			int recvResult;
			recvResult = recv(playerSocket, buf, bufSize, 0);
			int recvError;
			if (recvResult == 0 || recvResult == SOCKET_ERROR) {
				recvError = WSAGetLastError();
				if (recvError == 0 || recvError == 10054) {
					disconnect(player);
				}
				else if(recvError != WSAEWOULDBLOCK){
					wprintf(L"recv Error : %d\n", recvError);
					return false;
				}
				
			}
			player->recvBuf.push(recvResult, (BYTE*)buf);
			packetProcess(player);
		}
	}
	return true;
}

void networkSend() {

	FD_SET writeSet;
	FD_ZERO(&writeSet);

	stPlayer* player;
	stPlayer* playerStart = players;
	stPlayer* playerEnd = playerStart + MAX_PLAYER_NUM;

	for (player = playerStart; player != playerEnd; ++player) {
		if (player->id == 0 || player->sendBuf.size() == 0) {
			continue;
		}
		FD_SET(player->sock, &writeSet);
	}

	int selectResult;
	timeval delayTime;
	delayTime.tv_sec = 0;
	delayTime.tv_usec = 0;
	selectResult = select(0, nullptr, &writeSet, nullptr, &delayTime);

	if (selectResult > 0) {

		stPlayer* player;
		stPlayer* playerStart = players;
		stPlayer* playerEnd = playerStart + MAX_PLAYER_NUM;

		int sendResult;
		int sendError;

		for (player = playerStart; player != playerEnd; ++player) {

			if (FD_ISSET(player->sock, &writeSet) != 0) {

				CRingBuffer* sendBuf = &player->sendBuf;
				int bufSize = sendBuf->size();

				char* buf = (char*)malloc(bufSize);

				sendBuf->front(bufSize, (BYTE*)buf);
				sendBuf->pop(bufSize);

				sendResult = send(player->sock, buf, bufSize, 0);
				if (sendResult == SOCKET_ERROR) {
					sendError = WSAGetLastError();
					wprintf(L"send error : %d\n", sendError);
					return;
				}

				free(buf);

			}

		}

	}

}

void gameLogic() {
}

int main() {

	do {
		bool networkInitResult = networkInit();
		if (networkInitResult == false) {
			break;
		}
		bool initResult;
		initResult = init();
		if (initResult == false) {
			break;
		}

		bool recvResult;

		for (;;) {

			recvResult = networkRecv();
			if (recvResult == false) {
				break;
			}
			networkSend();
			gameLogic();
			networkSend();
		}

	} while (false);


	system("PAUSE>NUL");

	closesocket(listenSocket);
	WSACleanup();

	return 0;

}