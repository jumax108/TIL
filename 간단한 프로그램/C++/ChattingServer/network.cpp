#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")

#include "network.h"

const wchar_t* SERVER_IP = L"0.0.0.0";
constexpr unsigned short SERVER_PORT = 8000;

bool CNetwork::init() {

	WSADATA wsaData;

	int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	int wsaStartupError;
	
	if (wsaStartupResult != NULL) {
		wsaStartupError = WSAGetLastError();
		wprintf(L"wsa startup error : %d\n", wsaStartupError);
		return;
	}
	
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	int listenSocketError;
	if (listenSocket == INVALID_SOCKET) {
		listenSocketError = WSAGetLastError();
		wprintf(L"listen socket error : %d\n", listenSocketError);
		return;
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
		return;
	}

	int listenResult = listen(listenSocket, SOMAXCONN);
	int listenError;
	if (listenResult == SOCKET_ERROR) {
		listenError = WSAGetLastError();
		wprintf(L"listen error : %d\n", listenError);
		return;
	}

	u_long on = 1;
	int ioctlResult = ioctlsocket(listenSocket, FIONBIO, &on);
	int ioctlError;
	if (ioctlResult == SOCKET_ERROR) {
		wprintf(L"ioctl error : %d\n", ioctlError);
		return;
	}

}

SOCKET CNetwork::acceptConnect(SOCKADDR_IN* clientAddr) {

	int addrLen = sizeof(SOCKADDR_IN);
	return accept(listenSocket, (SOCKADDR*)clientAddr, &addrLen);

}

void recvPacket() {

	FD_SET readSet;
	int userCnt = 0;

	list<stUser*> userInSet;
	timeval delayZero;
	ZeroMemory(&delayZero, sizeof(timeval));

	for (unordered_map<SOCKET, stUser*>::iterator userListIter = userListBegin(); userListIter != userListEnd(); ++userListIter) {

		std::pair<SOCKET, stUser*> pair = *userListIter;
		SOCKET socket = pair.first;
		stUser* user = pair.second;

		FD_SET(socket, &readSet);
		++userCnt;

		userInSet.push_back(user);

		if (userCnt < 64) {
			continue;
		}

		userCnt = 0;
		
		int selectResult = select(0, &readSet, nullptr, nullptr, &delayZero);

		if (selectResult == 0) {
			continue;
		}

		for (list<stUser*>::iterator userSetIter = userInSet.begin(); userSetIter != userInSet.end(); ++userSetIter) {

			stUser* user = *userSetIter;
			SOCKET socket = user->_socket;
			CRingBuffer* recvBuffer = user->_recvBuffer;

			if (FD_ISSET(socket, &readSet) == false) {
				continue;
			}

			recv(socket, recvBuffer->getDirectPush(), recvBuffer->getDirectFreeSize(), 0);

		}

		FD_ZERO(&readSet);
		userInSet.clear();

	}

	do {
		if (userInSet.empty() == true) {
			break;
		}

		int selectResult = select(0, &readSet, nullptr, nullptr, &delayZero);
		if (selectResult == 0) {
			break;
		}
		for (list<stUser*>::iterator userSetIter = userInSet.begin(); userSetIter != userInSet.end(); ++userSetIter) {

			stUser* user = *userSetIter;
			SOCKET socket = user->_socket;
			CRingBuffer* recvBuffer = user->_recvBuffer;

			if (FD_ISSET(socket, &readSet) == false) {
				continue;
			}

			recv(socket, recvBuffer->getDirectPush(), recvBuffer->getDirectFreeSize(), 0);


		}
	} while (false);

	userInSet.clear();

}

void sendPacket() {
	FD_SET writeSet;
	int userCnt = 0;

	list<stUser*> userInSet;
	timeval delayZero;
	ZeroMemory(&delayZero, sizeof(timeval));

	for (unordered_map<SOCKET, stUser*>::iterator userListIter = userListBegin(); userListIter != userListEnd(); ++userListIter) {

		std::pair<SOCKET, stUser*> pair = *userListIter;
		SOCKET socket = pair.first;
		stUser* user = pair.second;

		FD_SET(socket, &writeSet);
		++userCnt;

		userInSet.push_back(user);

		if (userCnt < 64) {
			continue;
		}

		userCnt = 0;

		int selectResult = select(0, nullptr, &writeSet, nullptr, &delayZero);

		if (selectResult == 0) {
			continue;
		}

		for (list<stUser*>::iterator userSetIter = userInSet.begin(); userSetIter != userInSet.end(); ++userSetIter) {

			stUser* user = *userSetIter;
			SOCKET socket = user->_socket;
			CRingBuffer* sendBuffer = user->_sendBuffer;

			if (FD_ISSET(socket, &writeSet) == false) {
				continue;
			}

			send(socket, sendBuffer->getDirectPush(), sendBuffer->getDirectFreeSize(), 0);

		}

		FD_ZERO(&writeSet);
		userInSet.clear();

	}

	do {
		if (userInSet.empty() == true) {
			break;
		}

		int selectResult = select(0, nullptr, &writeSet, nullptr, &delayZero);

		if (selectResult == 0) {
			break;
		}

		for (list<stUser*>::iterator userSetIter = userInSet.begin(); userSetIter != userInSet.end(); ++userSetIter) {

			stUser* user = *userSetIter;
			SOCKET socket = user->_socket;
			CRingBuffer* sendBuffer = user->_sendBuffer;

			if (FD_ISSET(socket, &writeSet) == false) {
				continue;
			}

			send(socket, sendBuffer->getDirectPush(), sendBuffer->getDirectFreeSize(), 0);

		}
	} while (false);

	userInSet.clear();
}