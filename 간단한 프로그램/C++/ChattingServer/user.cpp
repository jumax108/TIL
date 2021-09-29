#include <unordered_map>
#include <WinSock2.h>

using std::unordered_map;

#pragma comment(lib, "ws2_32")

#include "ringBuffer.h"
#include "user.h"

stUser::stUser(SOCKET socket, SOCKADDR_IN* addr) {
	_socket = socket;
	memcpy(&_addr, addr, sizeof(SOCKADDR_IN));
	_id = 0;
	_nameLen = 0;
	_name = nullptr;

	_recvBuffer = new CRingBuffer(1000);
	_sendBuffer = new CRingBuffer(1000);
}

stUser::~stUser() {
	delete[] _name;
}

inline unordered_map<SOCKET, stUser*>::iterator userListBegin() {
	return userList.begin();
}

inline unordered_map<SOCKET, stUser*>::iterator userListEnd() {
	return userList.end();
}

void userListInsert(SOCKET socket, stUser* user) {
	userList.insert(std::pair<SOCKET, stUser*>(socket, user));
}