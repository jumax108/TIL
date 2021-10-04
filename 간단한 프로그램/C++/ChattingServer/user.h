#pragma once


using std::unordered_map;
using std::unordered_set;

class CUserList;
class CRoomList;

extern CUserList* userList;
extern CRoomList* roomList;

struct stUser {

public:

	stUser();
	stUser(SOCKET socket, SOCKADDR_IN* addr);
	~stUser();

	SOCKET _socket;
	SOCKADDR_IN _addr;

	unsigned int _id;

	unsigned short _nameLen;
	wchar_t* _name;

	CRingBuffer* _sendBuffer;
	CRingBuffer* _recvBuffer;

	unsigned int _roomId;
	bool _isErase = false;

};

static class wcharHash {
public:
	size_t operator()(wchar_t* str1) const {

		unsigned int hash = 0;

		int len = wcslen(str1);
		int byte = len * 2 ;

		while (byte > 0) {
			hash = (hash + ((char*)str1)[byte - 1]) % (unsigned int)0xFFFFFFFF;
			byte -= 1;
		}

		return hash;

	}
};

static class wcharEqualTo {

public : 
	bool operator()(wchar_t* str1, wchar_t* str2) const {

		return wcscmp(str1, str2) == 0;

	}

};

class CUserList {

public:
	unsigned int _idBase = 1;

	using USERNAME_LIST_TYPE = unordered_set<wchar_t*, wcharHash, wcharEqualTo>;
	using USER_LIST_TYPE = unordered_map<SOCKET, stUser*>;

	USERNAME_LIST_TYPE::iterator userNameListBegin();
	USERNAME_LIST_TYPE::iterator userNameListEnd();
	void userNameListInsert(wchar_t* name);
	bool isNameInList(wchar_t* name);

	USER_LIST_TYPE::iterator userListBegin();
	USER_LIST_TYPE::iterator userListEnd();
	void userListInsert(SOCKET, stUser*);
	unsigned char userListSize();
	CRingBuffer* getUserSendBuffer(USER_LIST_TYPE::iterator);
	CRingBuffer* getUserRecvBuffer(USER_LIST_TYPE::iterator);
	stUser* getUser(USER_LIST_TYPE::iterator);
	SOCKET getUserSocket(USER_LIST_TYPE::iterator);

	void eraseUser(stUser* user);
	void eraseName(wchar_t* name);


private:

	USERNAME_LIST_TYPE userNameList;
	USER_LIST_TYPE userList;


};
