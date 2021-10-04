#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <windows.h>
#pragma comment(lib, "ws2_32")
#define STRESS_TEST

#include "ringBuffer.h"
#include "protocolBuffer.h"
#include "user.h"
#include "room.h"

#include "network.h"
#include "common.h"
#include "recv.h"
#include "send.h"
#include "recvFunc.h"


using std::unordered_map;

CUserList* userList = new CUserList();
CRoomList* roomList = new CRoomList();
CStubFunc* stubFunc = new CStubFunc();
CProxyFunc* recvFunc = new CProxyFunc();

CNetwork<CUserList::USER_LIST_TYPE::iterator>* network = new CNetwork<CUserList::USER_LIST_TYPE::iterator>();

using std::list;

void acceptConnect();

unsigned int echoProcCnt = 0;
unsigned int startTime = 0;

int main() {

	bool networkInitResult = network->init();

	if (networkInitResult == false) {
		wprintf(L"network init fail\n");
		return 1;
	}

	wprintf(L"server open\n");

	timeBeginPeriod(1);
	startTime = timeGetTime();

	while (1) {

		acceptConnect();
		network->recvPacket();
		network->sendPacket();
		for (CUserList::USER_LIST_TYPE::iterator userIter = userList->userListBegin(); userIter != userList->userListEnd();) {
			stUser* user = userList->getUser(userIter);
			++userIter;
			if (user->_isErase == true){
				network->disconnect(user, user->_name != nullptr);
				continue;
			}
		}

#ifdef STRESS_TEST
		int time = timeGetTime();
		if (time - startTime >= 1000) {
			startTime = time;
			wprintf(L"process Cnt : %d\n", echoProcCnt);
			echoProcCnt = 0;
		}
#endif

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
		
		wprintf(L"connect user: %x(%d)\n", clientAddr.sin_addr, clientAddr.sin_port);

		stUser* user = new stUser(resSock, &clientAddr);
		userList->userListInsert(resSock, user);
		 
	}
}

