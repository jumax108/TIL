#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <list>
#include <unordered_map>
#pragma comment(lib, "ws2_32")


#include "network.h"
#include "ringBuffer.h"
#include "user.h"
#include "room.h"

#include "protocolBuffer.h"
#include "chatingProtocolBuffer.h"
#include "common.h"
#include "recv.h"
#include "send.h"

CNetwork* network;

using std::list;

void acceptConnect();
void recvPacket();
void sendPacket();

int main() {

	bool networkInitResult = network->init();

	if (networkInitResult == false) {
		wprintf(L"network init fail\n");
		return 1;
	}

	wprintf(L"server open\n");

	while (1) {

		acceptConnect();
		recvPacket();
		sendPacket();

	}

	return 0;

}

void acceptConnect() {

	SOCKET resSock; 
	for (;;) {
		SOCKADDR_IN clientAddr;
		resSock = network->acceptConnect(&clientAddr);

		if (resSock == INVALID_SOCKET) {
			break;
		}

		// 연결 후 초기화
		
		stUser* user = new stUser(resSock, &clientAddr);
		userListInsert(resSock, user);
		 
	}
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