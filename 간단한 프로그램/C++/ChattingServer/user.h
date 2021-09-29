#pragma once

class stUser;

using std::unordered_map;
unordered_map<SOCKET, stUser*> userList;

class stUser {

public:

	stUser(SOCKET socket, SOCKADDR_IN* addr);
	~stUser();

	SOCKET _socket;
	SOCKADDR_IN _addr;

	unsigned int _id;

	unsigned short _nameLen;
	wchar_t* _name;

	CRingBuffer* _sendBuffer;
	CRingBuffer* _recvBuffer;



};

unordered_map<SOCKET, stUser*>::iterator userListBegin();
unordered_map<SOCKET, stUser*>::iterator userListEnd();
void userListInsert(SOCKET, stUser*);