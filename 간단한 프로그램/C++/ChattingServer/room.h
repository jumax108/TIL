#pragma once

struct stRoom {

	unsigned int _id;
	unsigned short _nameLen;
	wchar_t* _name;

	unsigned char _userNum;
	using USERLIST_TYPE = unordered_map<unsigned int, stUser*>;
	USERLIST_TYPE _userList;
	stRoom::USERLIST_TYPE::iterator userListBegin();
	stRoom::USERLIST_TYPE::iterator userListEnd();

	stRoom(unsigned int id, unsigned short nameLen, wchar_t* name);
	~stRoom();
};
