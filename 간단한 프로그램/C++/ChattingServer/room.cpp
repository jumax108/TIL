#include <string>
#include <unordered_map>

#include "user.h"
#include "room.h"

using std::unordered_map;

stRoom::stRoom(unsigned int id, unsigned short nameLen, wchar_t* name) {
	_id = id;
	_nameLen = nameLen;
	
	_name = new wchar_t[_nameLen];
	wcscpy_s(_name, _nameLen, name);
}

stRoom::~stRoom() {
	delete[] _name;
}

stRoom::USERLIST_TYPE::iterator stRoom::userListBegin() {
	return _userList.begin();
}