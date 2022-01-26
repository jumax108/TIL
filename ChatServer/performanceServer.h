#pragma once

#include <unordered_set>

#include "./lib/common/lockFreeQueue.h"
#include "./lib/LanServer/lanServer.h"

class CPerformanceServer: public MyLanServer::CLanServer{

public:

	struct stMsg{
		unsigned short type;
		unsigned __int64 sessionID;
	};
	
	struct stPerformanceData{
		unsigned __int64 min = 0xFFFFFFFFFFFFFFFF;
		unsigned __int64 max = 0;
		unsigned __int64 sum = 0;
	};

	CPerformanceServer(){
		msgEvent = (HANDLE)CreateEvent(nullptr, true, false, nullptr);
	}

	virtual bool onConnectRequest(unsigned int ip, unsigned short port){
		return true;
	}

	virtual void onClientJoin(unsigned int ip, unsigned short port, unsigned __int64 sessionID){
		
		stMsg msg;
		msg.type = 1;
		msg.sessionID = sessionID;

		msgQue.push(msg);
		SetEvent(msgEvent);

	}

	virtual void onClientLeave(unsigned __int64 sessionID){
		
		stMsg msg;
		msg.type = 2;
		msg.sessionID = sessionID;

		msgQue.push(msg);
		SetEvent(msgEvent);

	}

	virtual void onRecv(unsigned __int64 sessionID, CPacketPtr packet){
		
	}

	virtual void onSend(unsigned __int64 sessionID, int sendSize){
		
	}

	virtual void onError(int errorCode, const wchar_t* errorMsg){
		
	}

	static unsigned __stdcall update(void* args);
	
	HANDLE _updateThread;

private:

	

	HANDLE msgEvent;
	CLockFreeQueue<stMsg> msgQue;

	std::unordered_set<unsigned __int64> userList;

	stPerformanceData recvTPS;
	stPerformanceData sendTPS;
	stPerformanceData acceptTPS;
	stPerformanceData updateTPS;
	stPerformanceData acceptTotal;

	stPerformanceData msgQueSize;
	stPerformanceData msgQueCap;

	stPerformanceData packetPoolCap;
	stPerformanceData iocpQueueSize;

	stPerformanceData cpuUsage;
	stPerformanceData processCpuKernelUsage;
	stPerformanceData processCpuUserUsage;

	stPerformanceData chatRecvMsgNum;
	stPerformanceData chatSendMsgNum;

	stPerformanceData sendBytes;
	stPerformanceData recvBytes;

	stPerformanceData sessionNum;
	stPerformanceData userNum;

	stPerformanceData processPrivateMemory;
	stPerformanceData nonPagedPoolMemory;

};