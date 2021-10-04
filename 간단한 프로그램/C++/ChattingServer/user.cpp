#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <list>
#include <unordered_set>
#include <unordered_map>
#pragma comment(lib, "ws2_32")

using std::unordered_map;
using std::unordered_set;

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

stUser::stUser() {
	_socket = NULL;
	_id = 0;
	_nameLen = 0;
	_roomId = 0;
	_name = nullptr;
	_recvBuffer = nullptr;
	_sendBuffer = nullptr;
}
stUser::stUser(SOCKET socket, SOCKADDR_IN* addr) {
	_socket = socket;
	memcpy(&_addr, addr, sizeof(SOCKADDR_IN));
	_id = 0;
	_nameLen = 0;
	_roomId = 0;
	_name = nullptr;

	_recvBuffer = new CRingBuffer(5000);
	_sendBuffer = new CRingBuffer(5000);
}

stUser::~stUser() {
	delete[] _name;
}

CUserList::USER_LIST_TYPE::iterator CUserList::userListBegin() {
	return userList.begin();
}

CUserList::USER_LIST_TYPE::iterator CUserList::userListEnd() {
	return userList.end();
}

void CUserList::userListInsert(SOCKET socket, stUser* user) {
	userList.insert(std::pair<SOCKET, stUser*>(socket, user));
}

CUserList::USERNAME_LIST_TYPE::iterator CUserList::userNameListBegin() {
	return userNameList.begin();
}

CUserList::USERNAME_LIST_TYPE::iterator CUserList::userNameListEnd() {
	return userNameList.end();
}

bool CUserList::isNameInList(wchar_t* name) {

	USERNAME_LIST_TYPE::iterator resultIter = userNameList.find(name);

	return resultIter != userNameList.end(); 
}

void CUserList::userNameListInsert(wchar_t* in) {

	wchar_t* name = new wchar_t[15];
	memcpy(name, in, 30);

	userNameList.insert(name);

}

unsigned char CUserList::userListSize() {

	return userList.size();

}

CRingBuffer* CUserList::getUserSendBuffer(USER_LIST_TYPE::iterator iter) {
	return (*iter).second->_sendBuffer;
}

CRingBuffer* CUserList::getUserRecvBuffer(USER_LIST_TYPE::iterator iter) {
	return (*iter).second->_recvBuffer;
}

SOCKET CUserList::getUserSocket(USER_LIST_TYPE::iterator iter) {
	return (*iter).first;
}

stUser* CUserList::getUser(USER_LIST_TYPE::iterator iter) {
	return (*iter).second;
}

void CUserList::eraseUser(stUser* user) {

	USER_LIST_TYPE::iterator userIter = userList.find(user->_socket);

	if (userIter == userList.end()) {
		return;
	}

	if (user->_roomId != 0) {

		roomList->leaveRoom(user);

	}
	userList.erase(userIter);

}

void CUserList::eraseName(wchar_t* name) {


	USERNAME_LIST_TYPE::iterator userNameIter = userNameList.find(name);
	userNameList.erase(userNameIter);
}