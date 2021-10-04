#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <list>
#include <unordered_set>
#include <unordered_map>
#pragma comment(lib, "ws2_32")
#include <string>
#include <unordered_set>
#include <unordered_map>

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

stRoom::stRoom(unsigned int id, unsigned short nameLen, wchar_t* name) {
	_id = id;
	_nameLen = nameLen;
	_userNum = 0;
	_name = new wchar_t[_nameLen + 1];
	memcpy(_name, name, nameLen * 2);
	_name[_nameLen] = '\0';
}

stRoom::~stRoom() {
	_userList.clear();
	delete[] _name;
}

stRoom::USERLIST_TYPE::iterator stRoom::userListBegin() {
	return _userList.begin();
}

stRoom::USERLIST_TYPE::iterator stRoom::userListEnd() {
	return _userList.end();
}

void stRoom::userListInsert(unsigned int id, stUser* user) {

	_userList.insert(std::pair<unsigned int, stUser*>(id, user));
	_userNum += 1;
}

CRoomList::ROOM_LIST_TYPE::iterator CRoomList::roomListBegin() {
	return roomList.begin();
}

CRoomList::ROOM_LIST_TYPE::iterator CRoomList::roomListEnd() {
	return roomList.end();
}

size_t CRoomList::roomListSize() {
	return roomList.size();
}

unsigned char stRoom::userListSize() {
	return _userList.size();
}

CRoomList::ROOM_NAME_LIST_TYPE::iterator CRoomList::roomNameListBegin() {
	return roomNameList.begin();
}

CRoomList::ROOM_NAME_LIST_TYPE::iterator CRoomList::roomNameListEnd() {
	return roomNameList.end();
}

bool CRoomList::isRoomNameInList(wchar_t* name) {
	return roomNameList.find(name) != roomNameList.end();
}

void CRoomList::roomNameListInsert(wchar_t* name) {
	roomNameList.insert(name);
}

void CRoomList::roomListInsert(stRoom* room) {
	roomList.insert(std::pair<unsigned int, stRoom*>(room->_id, room));
}

stRoom* CRoomList::getRoom(unsigned int id) {
	return roomList.find(id)->second;
}

void stRoom::leaveRoom(unsigned int userId) {

	USERLIST_TYPE::iterator findIter = _userList.find(userId);

	if (findIter == _userList.end()) {
		return;
	}

	_userList.erase(findIter);

	_userNum -= 1;
}

void CRoomList::eraseRoom(unsigned int roomId) {

	ROOM_LIST_TYPE::iterator findIter = roomList.find(roomId);

	if (findIter == roomList.end()) {
		return;
	}

	stRoom* room = (*findIter).second;

	ROOM_NAME_LIST_TYPE::iterator findNameIter = roomNameList.find(room->_name);

	roomList.erase(findIter);
	roomNameList.erase(findNameIter);

}

bool CRoomList::leaveRoom(stUser* user) {
	if (user->_roomId == 0) {
		return false;
	}

	stRoom* room = getRoom(user->_roomId);

	room->leaveRoom(user->_id);

}