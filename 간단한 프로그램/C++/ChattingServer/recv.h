#pragma once

#define STRESS_TEST

class CProxyFunc;
extern CProxyFunc* recvFunc;

class CProxyFuncBase{
public:
	virtual bool REQ_LogInProxy(wchar_t* name){ return false; }
	virtual bool RES_LogInProxy(unsigned char result, unsigned int userId){ return false; }
	virtual bool REQ_RoomListProxy(){ return false; }
	virtual bool RES_RoomListProxy(unsigned short roomNum, unsigned int* roomId, unsigned short* roomNameLen, wchar_t** roomName, unsigned char* userNum, wchar_t*** userName){ return false; }
	virtual bool REQ_RoomCreateProxy(unsigned short nameSize, wchar_t* name){ return false; }
	virtual bool RES_RoomCreateProxy(unsigned char result, unsigned int roomId, unsigned short roomNameSize, wchar_t* roomName){ return false; }
	virtual bool REQ_RoomEnterProxy(unsigned int roomId){ return false; }
	virtual bool RES_RoomEnterProxy(unsigned char result, unsigned int roomId, unsigned short roomNameSize, wchar_t* roomName, unsigned char userNum, wchar_t** userName, unsigned int* userId){ return false; }
	virtual bool REQ_ChatProxy(unsigned short msgSize, wchar_t* msg){ return false; }
	virtual bool REQ_RoomLeaveProxy(){ return false; }
	virtual bool RES_RoomDeleteProxy(){ return false; }
	virtual bool RES_UserEnterProxy(wchar_t* name, unsigned int id){ return false; }
	virtual bool REQ_STRESS_ECHOProxy(unsigned short size, char* str){ return false; }
	virtual bool RES_STRESS_ECHOProxy(unsigned short size, char* str){ return false; }
	void packetProc(stHeader* header, CProtocolBuffer* payload, CProxyFuncBase* proxy);
};
