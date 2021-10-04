#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <list>
#include <unordered_set>
#include <unordered_map>
#pragma comment(lib, "ws2_32")

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
using std::unordered_set;

bool CProxyFunc::REQ_LogInProxy(wchar_t* name) {

	bool isDuplicated = userList->isNameInList(name);
	unsigned char result = 4;
	unsigned int userId;

	if (isDuplicated == false) {
		userList->userNameListInsert(name);
		user->_id = userList->_idBase++;
		user->_name = new wchar_t[15];
		memcpy(user->_name, name, 30);
		result = 1;
	}
	else {
		user->_isErase = true;
		result = 2;
	}

	stubFunc->RES_LogInStub(user, result, user->_id);

	wprintf(L"[RECV] LogIn: %s\n", name);

	return true;

}

bool CProxyFunc::REQ_RoomListProxy() {

	stubFunc->RES_RoomListStub(user);
	wprintf(L"[RECV] Room List \n");
	return true;

}

bool CProxyFunc::REQ_RoomCreateProxy(unsigned short nameSize, wchar_t* name) {

	if (user->_id == 0) {
		user->_isErase = true;
		return false;
	}

	bool isDuplicated = roomList->isRoomNameInList(name);
	unsigned char result = 4;
	int roomId = 0;

	if (isDuplicated == false) {
		roomList->roomNameListInsert(name);
		roomId = roomList->baseRoomId;
		roomList->roomListInsert(new stRoom(roomList->baseRoomId++, nameSize, name));
		result = 1;
	}
	else {
		result = 2;
	}

	stubFunc->RES_RoomCreateStub(user, result, roomId, nameSize, name);
	wprintf(L"[RECV] Room Create, result: %d, roomId: %d\n", result, roomId);
	return true;

}

bool CProxyFunc::REQ_RoomEnterProxy(unsigned int roomId) {

	if (user->_id == 0) {
		user->_isErase = true;
		return false;
	}

	stRoom* room = roomList->getRoom(roomId);

	unsigned char result = 4;

	room->userListInsert(user->_id, user);
	result = 1;
	user->_roomId = roomId;

	stubFunc->RES_RoomEnterStub(user, result, roomId);
	stubFunc->RES_UserEnterStub(user, roomId);

	wprintf(L"[RECV] Room Enter : %d\n", roomId);
	return true;
}

bool CProxyFunc::REQ_ChatProxy(unsigned short msgSize, wchar_t* msg) {

	if (user->_id == 0 || user->_roomId == 0) {
		user->_isErase = true;
		return false;
	}

	stRoom* room = roomList->getRoom(user->_roomId);

	stubFunc->RES_ChatStub(user, user->_id, msgSize, msg);

	wprintf(L"[RECV] Chat : userId: %d\n", user->_id);

	return true;

}

bool CProxyFunc::REQ_RoomLeaveProxy() {

	stRoom* room = roomList->getRoom(user->_roomId);
	stubFunc->RES_RoomLeaveStub(user, user->_id);
	room->leaveRoom(user->_id);

	if (room->_userNum == 0) {

		stubFunc->RES_RoomDeleteStub(user, room->_id);
		roomList->eraseRoom(room->_id);

	}

	user->_roomId = 0;
	
	wprintf(L"[RECV] Room Leave : %d\n", room->_id);

	return true;
}
bool CProxyFunc::REQ_STRESS_ECHOProxy(unsigned short size, char* str) {

	stubFunc->RES_STRESS_ECHOStub(user, size, str);
#ifndef STRESS_TEST
	wprintf(L"[RECV] ECHO, userId: %d, size: %d\n", user->_id, size);
#endif

	return true;

}