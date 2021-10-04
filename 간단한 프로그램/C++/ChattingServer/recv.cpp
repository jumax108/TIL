#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <list>
#include <unordered_set>
#include <unordered_map>
#pragma comment(lib, "ws2_32")

#include "ringBuffer.h"
#include "protocolBuffer.h"
#include "user.h"
#include "room.h"

#include "network.h"
#include "common.h"
#include "recv.h"
#include "send.h"

#include "recvFunc.h"

void CProxyFuncBase::packetProc(stHeader* header, CProtocolBuffer* payload, CProxyFuncBase* proxy) {
	bool funcResult = false;
	switch (header->payloadType) {
	case REQ_LogIn:
	{
		wchar_t name[15];
		payload->popDataW(15, name);
		funcResult = proxy->REQ_LogInProxy(name);
	}
	break;
	case REQ_RoomList:
	{
		funcResult = proxy->REQ_RoomListProxy();
	}
	break;
	case REQ_RoomCreate:
	{
		unsigned short nameSize;
		*payload >> nameSize;
		nameSize /= 2;
		wchar_t* name = new wchar_t[nameSize + 1];
		name[nameSize] = '\0';
		payload->popDataW(nameSize, name);
		funcResult = proxy->REQ_RoomCreateProxy(nameSize, name);
	}
	break;
	case REQ_RoomEnter:
	{
		unsigned int roomId;
		*payload >> roomId;
		funcResult = proxy->REQ_RoomEnterProxy(roomId);
	}
	break;
	case REQ_Chat:
	{
		unsigned short msgSize;
		*payload >> msgSize;
		msgSize /= 2;
		wchar_t* msg = new wchar_t[msgSize];
		payload->popDataW(msgSize, msg);
		funcResult = proxy->REQ_ChatProxy(msgSize, msg);
		delete[] msg;
	}
	break;
	case REQ_RoomLeave:
	{
		funcResult = proxy->REQ_RoomLeaveProxy();
	}
	break;
	case REQ_STRESS_ECHO:
	{
		unsigned short size;
		*payload >> size;

		if (size > 20000) {
			int k = 1;
		}

		char* str = new char[size];
		payload->popData(size, str);
		funcResult = proxy->REQ_STRESS_ECHOProxy(size, str);
		delete[] str;
	}
	break;
	}

}