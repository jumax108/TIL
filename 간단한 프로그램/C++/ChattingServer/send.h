#pragma once
#define STRESS_TEST

class CStubFunc;
extern CStubFunc* stubFunc;

class CUserList;
class CRoomList;

extern CUserList* userList;
extern CRoomList* roomList;

using std::unordered_map;
template<typename USER_LIST_ITER>
class CNetwork;
extern CNetwork<CUserList::USER_LIST_TYPE::iterator>* network;

extern unsigned int echoProcCnt;

class CStubFunc{
public:
	virtual bool RES_LogInStub(stUser* user, unsigned char result, unsigned int userId);
	virtual bool RES_RoomListStub(stUser* user);
	virtual bool RES_RoomCreateStub(stUser* user, unsigned char result, unsigned int roomId, unsigned short roomNameSize, wchar_t* roomName);
	virtual bool RES_RoomEnterStub(stUser* user, unsigned char result, unsigned int roomId);
	virtual bool RES_ChatStub(stUser* user, unsigned int userId, unsigned short msgSize, wchar_t* msg);
	virtual bool RES_RoomLeaveStub(stUser* user, unsigned int userId);
	virtual bool RES_RoomDeleteStub(stUser* user, unsigned int roomId);
	virtual bool RES_UserEnterStub(stUser* user, unsigned int roomId);
	virtual bool RES_STRESS_ECHOStub(stUser* user, unsigned short size, char* msg);

};
