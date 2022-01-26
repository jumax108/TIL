#pragma once

#include <unordered_map>
#include "./lib/NetServer/netServer.h"
#include "common.h"
#include "./lib/common/performanceData.h"

constexpr int SECTOR_WIDTH = 50;
constexpr int SECTOR_HEIGHT = 50;

struct stSector;

struct stUser{
	
    __int64 accountNo;
    WCHAR id[20];
    WCHAR nickName[20];
    char sessionKey[64];

	stSector* sector;
    unsigned __int64 lastSendTime;

};
struct stSector{

	std::unordered_map<unsigned __int64, stUser*> userList;
	unsigned char aroundSectorNum;
	stSector** aroundSector;

};

struct stMsg{

	CPacketPtr packet;
	short type;
    unsigned __int64 sessionID;

};

class CPerformanceServer;

class CChatServer: public CNetServer{

	friend class CPerformanceServer;

public:


	CChatServer();

	virtual bool onConnectRequest(unsigned int ip, unsigned short port);
	virtual void onClientJoin(unsigned int ip, unsigned short port, unsigned __int64 sessionID);
	virtual void onClientLeave(unsigned __int64 sessionID);

	virtual void onRecv(unsigned __int64 sessionID, CPacketPtr packet);
	virtual void onSend(unsigned __int64 sessionID, int sendSize);

	virtual void onError(int errorCode, const wchar_t* errorMsg);

	static unsigned __stdcall update(void*);
	static unsigned __stdcall tpsFunc(void*);
	static unsigned __stdcall logInHearthBeatSignalFunc(void*);
	static unsigned __stdcall notLogInHearthBeatSignalFunc(void*);
	
	unsigned __int64 getUpdateTPS(){return updateTPS;}

	void updateUserDispersion();

private:

	CLockFreeQueue<stMsg*> msgQueue;

	alignas(64) HANDLE msgEvent;
	HANDLE logInHeartBeatSignalThread;
	HANDLE notLogInHeartBeatSignalThread;
	HANDLE tpsPrintThread;

	alignas(64) std::unordered_map<unsigned __int64, stUser*> userList;
	std::unordered_map<unsigned __int64, stUser*> notLogInUserList;
	
	CObjectFreeList<stUser> _userFreeList;
	CObjectFreeList<stMsg> _msgFreeList;
	
	alignas(64) __int64 _chatMsgRecvNum = 0;
	__int64 _chatMsgSendNum = 0;
	__int64 _chatMsgRecvTPS = 0;
	__int64 _chatMsgSendTPS = 0;
	__int64 updateCnt;

	alignas(64) CPerformanceData _performanceData;

	alignas(64) stSector sectors[SECTOR_HEIGHT][SECTOR_WIDTH];
	__int64 _maxUserDispersion[5][5];
	__int64 _numUserDispersion[5][5];

	unsigned __int64 updateTPS;


	//CLanServer _performanceServer;
};