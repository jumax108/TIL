#pragma once
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <list>
#include <unordered_set>
#include <unordered_map>
#pragma comment(lib, "ws2_32")

#include <unordered_map>
#include <unordered_set>
#include <list>

using std::unordered_map;
using std::unordered_set;
using std::list;

#include "ringBuffer.h"
#include "protocolBuffer.h"
#include "user.h"
#include "room.h"

#include "network.h"
#include "common.h"
#include "recv.h"
#include "send.h"
#include "recvFunc.h"

static const wchar_t* SERVER_IP = L"0.0.0.0";
static constexpr unsigned short SERVER_PORT = 6000;

class CUserList;
class CRoomList;
class CProxyFunc;

extern CUserList* userList;
extern CRoomList* roomList;
extern CProxyFunc* recvFunc;

template<typename USER_LIST_ITER>
class CNetwork {


public:

	bool init();
	SOCKET acceptConnect(SOCKADDR_IN* clientAddr);

	void recvPacket();
	void sendPacket();

	void checkFullMessage(stUser* user);

	void uniCast(CRingBuffer* sendBuffer, unsigned int msgSize, CProtocolBuffer* msg);

	void broadCast(USER_LIST_ITER beginIter, USER_LIST_ITER endIter, stUser* exceptUser, unsigned int msgSize, CProtocolBuffer* msg);

	unsigned char getCheckSum(CProtocolBuffer* msg, unsigned short msgType);

	void disconnect(stUser* user, bool eraseName = true);

private:

	SOCKET listenSocket;

};

class CUserSet {

public:

	using LIST_TYPE = list<stUser*>;

	CUserSet() {
		userCnt = 0;
	}
	~CUserSet() {
		if (userCnt > 0) {
			if (isRecv == true) {
				recvInList();
			}
			else {
				sendInList();
			}
		}
	}

	unsigned int size() {
		return userCnt;
	}

	LIST_TYPE::iterator begin() {
		return userListInSet.begin();
	}

	LIST_TYPE::iterator end() {
		return userListInSet.end();
	}

	void clear() {
		FD_ZERO(&set);
		userListInSet.clear();
		userCnt = 0;
	}

	void insert(SOCKET socket, stUser* user) {
		FD_SET(socket, &set);
		userListInSet.push_back(user);
		userCnt += 1;
	}

	bool isSet(SOCKET socket) {
		return FD_ISSET(socket, &set);
	}

	int selectSet(timeval delay) {
		if (isRecv == true) {
			return select(0, &set, nullptr, nullptr, &delay);
		}
		return select(0, nullptr, &set, nullptr, &delay);

	}

	void recvInList() {
		for (LIST_TYPE::iterator userSetIter = userListInSet.begin(); userSetIter != userListInSet.end(); ++userSetIter) {

			stUser* user = *userSetIter;
			SOCKET socket = user->_socket;
			CRingBuffer* recvBuffer = user->_recvBuffer;

			if (isSet(socket) == false) {
				continue;
			}

			recvFunc->user = user;
			int recvSize;
			while (true) {
				recvSize = recv(socket, recvBuffer->getDirectPush(), recvBuffer->getDirectFreeSize(), 0);
				int recvError;
				if (recvSize == SOCKET_ERROR) {
					recvError = WSAGetLastError();
					if (recvError == WSAEWOULDBLOCK) {
						break;
					}
					else {
						wprintf(L"[RECV] errorCode: %d, userId: %d\n", recvError, user->_id);
						if (user->_id == 0) {
							int k = 1;
						}
						user->_isErase = true;
						break;
					}
				}
				recvBuffer->moveRear(recvSize);
			}
			network->checkFullMessage(user);
		}
	}
	void sendInList() {
		for (LIST_TYPE::iterator userSetIter = userListInSet.begin(); userSetIter != userListInSet.end(); ++userSetIter) {

			stUser* user = *userSetIter;
			SOCKET socket = user->_socket;
			CRingBuffer* sendBuffer = user->_sendBuffer;

			if (isSet(socket) == false) {
				continue;
			}

			int sendSize = send(socket, sendBuffer->getDirectFront(), sendBuffer->getDirectUsedSize(),  0);
			int sendError;
			if (sendSize == SOCKET_ERROR) {
				sendError = WSAGetLastError();
				wprintf(L"[SEND] ErrorCode: %d, userId: %d\n", sendError, user->_id);
				user->_isErase = true;
			}
			sendBuffer->moveFront(sendSize);

		}
	}

	bool isRecv = false;

private:

	FD_SET set;
	unsigned int userCnt;
	LIST_TYPE userListInSet;

};

template<typename USER_LIST_ITER>
bool CNetwork<USER_LIST_ITER>::init() {

	WSADATA wsaData;

	int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
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
	InetPtonW(AF_INET, SERVER_IP, &serverAddr.sin_addr.S_un.S_addr);
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

	return true;

}

template<typename USER_LIST_ITER>
SOCKET CNetwork<USER_LIST_ITER>::acceptConnect(SOCKADDR_IN* clientAddr) {

	int addrLen = sizeof(SOCKADDR_IN);
	return accept(listenSocket, (SOCKADDR*)clientAddr, &addrLen);

}

template<typename USER_LIST_ITER>
void CNetwork<USER_LIST_ITER>::recvPacket() {

	CUserSet userSet;
	userSet.isRecv = true;

	timeval delayZero;
	ZeroMemory(&delayZero, sizeof(timeval));

	for (unordered_map<SOCKET, stUser*>::iterator userListIter = userList->userListBegin(); userListIter != userList->userListEnd(); ++userListIter) {

		std::pair<SOCKET, stUser*> pair = *userListIter;
		SOCKET socket = pair.first;
		stUser* user = pair.second;

		userSet.insert(socket, user);

		if (userSet.size() < 64) {
			continue;
		}

		do {
			int selectResult = userSet.selectSet(delayZero);

			if (selectResult == 0) {
				break;
			}

			userSet.recvInList();

		} while (false);

		userSet.clear();
	}

	if (userSet.size() > 0) {
		do {
			int selectResult = userSet.selectSet(delayZero);

			if (selectResult == 0) {
				break;
			}

			userSet.recvInList();

		} while (false);
	}

}

template<typename USER_LIST_ITER>
void CNetwork<USER_LIST_ITER>::sendPacket() {

	CUserSet userSet;
	userSet.isRecv = false;

	timeval delayZero;
	ZeroMemory(&delayZero, sizeof(timeval));

	for (unordered_map<SOCKET, stUser*>::iterator userListIter = userList->userListBegin(); userListIter != userList->userListEnd(); ++userListIter) {

		std::pair<SOCKET, stUser*> pair = *userListIter;
		SOCKET socket = pair.first;
		stUser* user = pair.second;

		if (user->_sendBuffer->getUsedSize() == 0) {
			continue;
		}

		userSet.insert(socket, user);

		if (userSet.size() < 64) {
			continue;
		}

		do {
			int selectResult = userSet.selectSet(delayZero);

			if (selectResult == 0) {
				break;
			}

			userSet.sendInList();

		} while (false);

		userSet.clear();
	}
}

template<typename USER_LIST_ITER>
void CNetwork<USER_LIST_ITER>::uniCast(CRingBuffer* sendBuffer, unsigned int msgSize, CProtocolBuffer* msg) {

	while (msgSize > 0) {

		unsigned int directSize = sendBuffer->getDirectFreeSize();

		unsigned int pushSize = 0;

		if (directSize > msgSize) {
			pushSize = msgSize;
		}
		else {
			pushSize = directSize;
		}

		memcpy(sendBuffer->getDirectPush(), msg->getFrontPtr(), pushSize);
		sendBuffer->moveRear(pushSize);
		msgSize -= pushSize;
		msg->moveFront(pushSize);

	}

}

template<typename USER_LIST_ITER>
void CNetwork<USER_LIST_ITER>::broadCast(USER_LIST_ITER beginIter, USER_LIST_ITER endIter, stUser* exceptUser, unsigned int msgSize, CProtocolBuffer* msg) {

	for (USER_LIST_ITER iter = beginIter; iter != endIter; ++iter) {
		if (userList->getUser(iter) == exceptUser) {
			continue;
		}

		CProtocolBuffer msgTemp(msgSize);
		msgTemp.putData(msgSize, msg->getFrontPtr());

		uniCast(userList->getUserSendBuffer(iter), msgSize, &msgTemp);
	}

}

template<typename USER_LIST_ITER>
unsigned char CNetwork<USER_LIST_ITER>::getCheckSum(CProtocolBuffer* msg, unsigned short msgType) {

	unsigned char checkSum = msgType;
	for (unsigned char* iter = (unsigned char*)msg->getFrontPtr(); iter != (unsigned char*)msg->getRearPtr(); ++iter) {
		checkSum = (checkSum + *iter) % 256;
	}
	return checkSum;

}

template<typename USER_LIST_ITER>
void CNetwork<USER_LIST_ITER>::checkFullMessage(stUser* user) {

	CRingBuffer* recvBuffer = user->_recvBuffer;

	while (recvBuffer->getUsedSize() >= sizeof(stHeader)) {

		stHeader header;
		recvBuffer->front(sizeof(header), (char*)&header);
		
		if (recvBuffer->getUsedSize() < sizeof(stHeader) + header.payloadSize) {
			break;
		}
		recvBuffer->pop(sizeof(header));

		CProtocolBuffer payload(header.payloadSize);

		recvBuffer->front(header.payloadSize, payload.getRearPtr());
		recvBuffer->pop(header.payloadSize);
		payload.moveRear(header.payloadSize);

		recvFunc->packetProc(&header, &payload, recvFunc);
		

	}

}

template<typename USER_LIST_ITER>
void CNetwork<USER_LIST_ITER>::disconnect(stUser* user, bool eraseName) {

	closesocket(user->_socket);
	if (eraseName == true) {
		userList->eraseName(user->_name);
	}
	userList->eraseUser(user);
}