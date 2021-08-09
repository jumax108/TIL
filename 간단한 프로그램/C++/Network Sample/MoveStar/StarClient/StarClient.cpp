#include <stdio.h>
#include <WinSock2.h>
#include <locale.h>
#include <ws2tcpip.h>

#include "ringBuffer.h"
#include "myStack.h"
#include "queue.h"

#pragma comment(lib, "ws2_32")

enum class MESSAGE_TYPE {

	GET_ID = 0,
	CREATE_STAR,
	DELETE_STAR,
	MOVE_STAR

};

struct stMsgGetId {

	UINT type;
	UINT id;
	UCHAR notUsed[8];

};

struct stMsgCreateStar{

	UINT type;
	UINT id;
	UINT x;
	UINT y;

};

struct stMsgDeleteStar {

	UINT type;
	UINT id;
	UCHAR notUsed[8];

};

struct stMsgMoveStar {

	UINT type;
	UINT id;
	UINT x;
	UINT y;

};

struct stStar {

	UINT id;
	UINT x;
	UINT y;

};


CQueue<BYTE> keyBuffer(5);

my::stack<BYTE> starIndex(63);
stStar star[63];
stStar* myStar;
int tempId = -1;

WCHAR SERVER_IP[16] = { 0, };
constexpr USHORT SERVER_PORT = 3000;

SOCKET sock;

constexpr USHORT RINGBUFFER_SIZE = 1460 * 2;
CRingBuffer sendBuffer(RINGBUFFER_SIZE);
CRingBuffer recvBuffer(RINGBUFFER_SIZE);

bool networkInit() {

	WSAData wsaData;
	int startupErrorCode;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NULL) {

		startupErrorCode = WSAGetLastError();
		wprintf(L"wsa startup error : %d\n", startupErrorCode);
		return false;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	int socketErrorCode;
	if (sock == INVALID_SOCKET) {
		socketErrorCode = WSAGetLastError();
		wprintf(L"wsa socket error : %d\n", socketErrorCode);
		return false;

	}

	u_long setNonBlockingSocket = 1;
	int ioctlResult = ioctlsocket(sock, FIONBIO, &setNonBlockingSocket);
	int ioctlError;
	if (ioctlResult == SOCKET_ERROR) {

		ioctlError = WSAGetLastError();
		wprintf(L"wsa ioctl error : %d\n", ioctlError);
		return false;

	}

	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	InetPtonW(AF_INET, SERVER_IP, &serverAddr.sin_addr.s_addr);
	serverAddr.sin_port = htons(SERVER_PORT);
	int connectResult = connect(sock, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR_IN));
	int connectErrorCode;
	if (connectResult == SOCKET_ERROR) {

		connectErrorCode = WSAGetLastError();
		if (connectErrorCode != WSAEWOULDBLOCK) {
			wprintf(L"wsa connect error : %d\n", connectErrorCode);
			return false;
		}

	}

	return true;
}

bool isConnected = false;
bool network() {

	FD_SET readSet, writeSet, exceptSet;

	FD_ZERO(&readSet);
	FD_ZERO(&writeSet);
	FD_ZERO(&exceptSet);

	FD_SET(sock, &readSet);

	if (sendBuffer.size() > 0) {

		FD_SET(sock, &writeSet);

	} 

	if (isConnected == false) {

		FD_SET(sock, &exceptSet);
	}

	timeval waitTime;
	waitTime.tv_sec = 0;
	waitTime.tv_usec = 0;
	int selectResult = select(0, &readSet, &writeSet, &exceptSet, &waitTime);
	int selectErrorCode;
	if (selectResult == SOCKET_ERROR) {

		selectErrorCode = WSAGetLastError();
		wprintf(L"wsa select error : %d\n", selectErrorCode);
		return false;
	}

	////////////////////////////////////////
	// recv

	int recvResult;
	int recvErrorCode;
	constexpr USHORT RECV_BUF_SIZE = 255;
	char recvTempBuf[RECV_BUF_SIZE];
	if (FD_ISSET(sock, &readSet) == true) {

		recvResult = recv(sock, recvTempBuf, RECV_BUF_SIZE, 0);
		if (recvResult == 0 || recvResult == SOCKET_ERROR) {

			recvErrorCode = WSAGetLastError();
			if (recvErrorCode != WSAEWOULDBLOCK) {
				wprintf(L"recv error code : %d\n", recvErrorCode);
				return false;
			}
		}

		recvBuffer.push(recvResult, (const BYTE*)recvTempBuf);

	}
	////////////////////////////////////////


	////////////////////////////////////////
	// send

	int sendResult;
	int sendErrorCode;
	constexpr USHORT SEND_BUF_SIZE = 255;
	char sendTempBuf[SEND_BUF_SIZE];
	int sendSize = sendBuffer.size();
	if (sendSize > SEND_BUF_SIZE) {
		sendSize = SEND_BUF_SIZE;
	}
	if (FD_ISSET(sock, &writeSet) == true) {

		sendBuffer.front(sendSize, (BYTE*)sendTempBuf);
		sendBuffer.pop(sendSize);

		sendResult = send(sock, sendTempBuf, SEND_BUF_SIZE, 0);
		if (sendResult == 0 || sendResult == SOCKET_ERROR) {

			sendErrorCode = WSAGetLastError();
			if (sendErrorCode != WSAEWOULDBLOCK) {
				wprintf(L"recv error code : %d\n", sendErrorCode);
				return false;
			}
		}

	}
	/////////////////////////////////////////


	/////////////////////////////////////////
	// except
	if (FD_ISSET(sock, &exceptSet) == true) {
		
		wprintf(L"select except, connect error");
		return false;
		
	}
	/////////////////////////////////////////

	return true;

}

void logic() {

	////////////////////////////////////////////////////////////////////
	// 네트워크 메시지 처리
	BYTE msgData[16];
	int msgType;
	while (recvBuffer.size() > 0) {

		recvBuffer.front(16, msgData);
		recvBuffer.pop(16);

		msgType = *(int*)msgData;
		switch ((MESSAGE_TYPE)msgType) {
		case MESSAGE_TYPE::GET_ID: 
			{
				stMsgGetId* msg = (stMsgGetId*)msgData;
				tempId = msg->id;
			}
			break;
		case MESSAGE_TYPE::CREATE_STAR: 
			{
				stMsgCreateStar* msg = (stMsgCreateStar*)msgData;
				int useStarIndex;
				stStar* newStar;

				useStarIndex = starIndex.front();
				starIndex.pop();
				newStar = &star[useStarIndex];
				newStar->id = msg->id;
				newStar->x = msg->x;
				newStar->y = msg->y;
			}
			break;
		case MESSAGE_TYPE::DELETE_STAR: 
			{
				stMsgDeleteStar* msg = (stMsgDeleteStar*)msgData;

				int delStarId = msg->id;
				stStar* nowStar = star;
				for (BYTE starCnt = 0; starCnt < 63; ++starCnt, ++nowStar) {

					if (nowStar->id == delStarId) {
						nowStar->id = -1;
						starIndex.push(starCnt);
						break;
					}

				}
			}
			break;
		case MESSAGE_TYPE::MOVE_STAR: 
			{
				stMsgMoveStar* msg = (stMsgMoveStar*)msgData;

				int moveStarId = msg->id;
				stStar* nowStar = star;
				for (BYTE starCnt = 0; starCnt < 63; ++starCnt, ++nowStar) {

					if (nowStar->id == moveStarId) {

						nowStar->x = msg->x;
						nowStar->y = msg->y;

					}

				}
			}
			break;
		}

	}
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// 서버로 부터 ID 받으면 내 별이 어디에 저장되어 있는지 확인
	// 못찾으면 다음 프레임에도 검색 (찾을때까지)
	if (tempId != -1) {

		stStar* nowStar = star;
		for (BYTE starCnt = 0; starCnt < 63; ++starCnt, ++nowStar) {

			if (tempId == nowStar->id) {
				tempId = -1;
				myStar = nowStar;
				break;
			}

		}

	}
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// keyboard input logic
	bool moveThisFrame = false;
	while (keyBuffer.size() > 0) {

		BYTE keyCode;
		keyBuffer.front(&keyCode);
		keyBuffer.pop();

		switch (keyCode) {
		case VK_UP:
			if (myStar->y - 1 >= 0) {
				myStar->y -= 1;
			}
			moveThisFrame = true;
			break;
		case VK_DOWN:
			if (myStar->y + 1 < 23) {
				myStar->y += 1;
			}
			moveThisFrame = true;
			break;
		case VK_LEFT:
			if (myStar->x - 1 >= 0) {
				myStar->x -= 1;
			}
			moveThisFrame = true;
			break;
		case VK_RIGHT:
			if (myStar->x + 1 < 80) {
				myStar->x += 1;
			}
			moveThisFrame = true;
			break;
		}

	}
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////
	// 이동이 발생했으면 sendBuffer에 전달할 메시지 저장
	if (moveThisFrame == true) {

		stMsgMoveStar msg;
		msg.type = (UINT)MESSAGE_TYPE::MOVE_STAR;
		msg.id = myStar->id;
		msg.x = myStar->x;
		msg.y = myStar->y;
		sendBuffer.push(sizeof(stMsgMoveStar), (const BYTE*)&msg);

	}
	////////////////////////////////////////////////////////////////////

}

void init() {

	for (BYTE starCnt = 0; starCnt < 63; ++starCnt) {
		star[starCnt].id = -1;
	}

	for (BYTE starCnt = 0; starCnt < 63; starCnt++) {
		starIndex.push(starCnt);
	}


}

void render() {
	
	for (int colCnt = 0; colCnt < 23; colCnt++) {
		for (int rowCnt = 0; rowCnt < 80; rowCnt++) {
			stStar* nowStar = star;
			bool printBlank = true;
			for (int starCnt = 0; starCnt < 63; ++starCnt, ++nowStar) {
				if (nowStar->id == -1) {
					continue;
				}
				if (nowStar->x == rowCnt && nowStar->y == colCnt) {
					wprintf(L"*");
					printBlank = false;
					break;
				}
			}
			if (printBlank == true) {
				wprintf(L" ");
			}
		}
		printf("\n");
	}

}

#define GetKey(keyCode) \
	if (GetAsyncKeyState(keyCode)){ \
		keyBuffer.push(keyCode);	\
	}


void input() {

	GetKey(VK_UP);
	GetKey(VK_DOWN);
	GetKey(VK_LEFT);
	GetKey(VK_RIGHT);

}

int main() {

	setlocale(LC_ALL, "ko-KR");

	////////////////////////////////////////////////////////
	// SERVER IP 입력
	wprintf(L"IP를 입력하세요: ");
	wscanf_s(L"%s", SERVER_IP, (unsigned int)_countof(SERVER_IP));
	////////////////////////////////////////////////////////

	bool networkInitResult;
	networkInitResult = networkInit();
	if (networkInitResult == false) {
		return 0;
	}
	init();

	bool networkResult;

	while (1) {
		system("cls");
		input();
		networkResult = network();
		if (networkResult == false) {
			break;
		}
		logic();
		network();
		render();
	}

	closesocket(sock);

	WSACleanup();

	return 0;

}