#pragma once

class CRoomList;
class CUserList;

extern CUserList* userList;
extern CRoomList* roomList;

class wcharHash;
class wcharEqualTo;

struct stRoom {

	unsigned int _id;
	unsigned short _nameLen;
	wchar_t* _name;

	unsigned char _userNum;
	using USERLIST_TYPE = unordered_map<SOCKET, stUser*>;
	USERLIST_TYPE _userList;
	stRoom::USERLIST_TYPE::iterator userListBegin();
	stRoom::USERLIST_TYPE::iterator userListEnd();
	unsigned char userListSize();
	void userListInsert(SOCKET, stUser*);

	void leaveRoom(unsigned int userId);

	stRoom() {
		_id = 0;
		_nameLen = 0;
		_name = nullptr;
		_userNum = 0;
	}
	stRoom(unsigned int id, unsigned short nameLen, wchar_t* name);
	~stRoom();
};

class CRoomList {

public:
	unsigned int baseRoomId = 1;

	using ROOM_NAME_LIST_TYPE = unordered_set<wchar_t*, wcharHash, wcharEqualTo>;
	ROOM_NAME_LIST_TYPE::iterator roomNameListBegin();
	ROOM_NAME_LIST_TYPE::iterator roomNameListEnd();
	void roomNameListInsert(wchar_t* name);
	bool isRoomNameInList(wchar_t* name);

	using ROOM_LIST_TYPE = unordered_map<unsigned int, stRoom*>;

	ROOM_LIST_TYPE::iterator roomListBegin();
	ROOM_LIST_TYPE::iterator roomListEnd();
	void roomListInsert(stRoom* room);
	size_t roomListSize();
	stRoom* getRoom(unsigned int id);

	void eraseRoom(unsigned int roomId);

	bool leaveRoom(stUser* user);

private:
	ROOM_NAME_LIST_TYPE roomNameList;
	ROOM_LIST_TYPE roomList;
};
