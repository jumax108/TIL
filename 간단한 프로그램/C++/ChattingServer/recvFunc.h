#pragma once

class CStubFunc;
extern CStubFunc* stubFunc;


class CUserList;
class CRoomList;

extern CUserList* userList;
extern CRoomList* roomList;

class CProxyFunc : public CProxyFuncBase {

public:
	stUser* user;

	virtual bool REQ_LogInProxy(wchar_t* name);
	virtual bool REQ_RoomListProxy();
	virtual bool REQ_RoomCreateProxy(unsigned short nameSize, wchar_t* name);
	virtual bool REQ_RoomEnterProxy(unsigned int roomId);
	virtual bool REQ_ChatProxy(unsigned short msgSize, wchar_t* msg);
	virtual bool REQ_RoomLeaveProxy();
	virtual bool REQ_STRESS_ECHOProxy(unsigned short size, char* str);

};