#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <list>
#include <unordered_set>
#include <unordered_map>
#pragma comment(lib, "ws2_32")

#include <unordered_map>
#include <unordered_set>

using std::unordered_map;
using std::unordered_set;

#include "ringBuffer.h"
#include "protocolBuffer.h"
#include "user.h"
#include "room.h"

#include "network.h"
#include "common.h"
#include "recv.h"
#include "send.h"

#include "recvFunc.h"

bool CStubFunc::RES_LogInStub(stUser* user, unsigned char result, unsigned int userId) {

	CProtocolBuffer header(6);
	CProtocolBuffer payload(50);

	payload << result;
	payload << userId;

	header << (unsigned char)0x89;
	header << network->getCheckSum(&payload, RES_LogIn);
	header << (unsigned short)RES_LogIn;
	header << (unsigned short)payload.getUsedSize();

	network->uniCast(user->_sendBuffer, header.getUsedSize(), &header);
	network->uniCast(user->_sendBuffer, payload.getUsedSize(), &payload);

	wprintf(L"[SEND] Login, result: %d, userId: %d\n", result, userId);
	return true;
}

bool CStubFunc::RES_RoomListStub(stUser* user){

	CProtocolBuffer header(6);
	CProtocolBuffer payload(255);

	payload << (unsigned short)roomList->roomListSize();

	for (CRoomList::ROOM_LIST_TYPE::iterator roomListIter = roomList->roomListBegin(); roomListIter != roomList->roomListEnd(); ++roomListIter) {

		stRoom* room = (*roomListIter).second;

		payload << (*roomListIter).first;
		unsigned short roomNameLen = room->_nameLen;
		payload << (unsigned short)(roomNameLen * 2);
		payload.putDataW(roomNameLen, room->_name);

		unsigned char userNum = room->userListSize();
		payload << userNum;

		for (stRoom::USERLIST_TYPE::iterator userListIter = room->userListBegin(); userListIter != room->userListEnd(); ++userListIter) {
			payload.putDataW(15, (*userListIter).second->_name);
		}

	}

	header << (unsigned char)0x89;
	header << network->getCheckSum(&payload, RES_RoomList);
	header << (unsigned short)RES_RoomList;
	header << (unsigned short)payload.getUsedSize();

	network->uniCast(user->_sendBuffer, header.getUsedSize(), &header);
	network->uniCast(user->_sendBuffer, payload.getUsedSize(), &payload);
	wprintf(L"[SEND] RoomList\n");

	return true;
}

bool CStubFunc::RES_RoomCreateStub(stUser* user, unsigned char result, unsigned int roomId, unsigned short roomNameSize, wchar_t* roomName)
{
	CProtocolBuffer payload(50);
	payload << result;
	payload << roomId;
	payload << (unsigned short)(roomNameSize * 2);
	payload.putDataW(roomNameSize, roomName);

	CProtocolBuffer header(6);
	header << (unsigned char)0x89;
	header << (unsigned char)network->getCheckSum(&payload, RES_RoomCreate);
	header << (unsigned short)RES_RoomCreate;
	header << (unsigned short)payload.getUsedSize();

	network->broadCast(userList->userListBegin(), userList->userListEnd(), nullptr, header.getUsedSize(), &header);
	network->broadCast(userList->userListBegin(), userList->userListEnd(), nullptr, payload.getUsedSize(), &payload);
	wprintf(L"[SEND] RoomCreate, result: %d, roomId: %d\n", result, roomId);
	return true;
}

bool CStubFunc::RES_RoomEnterStub(stUser* user, unsigned char result, unsigned int roomId)
{
	CProtocolBuffer payload(50);
	payload << result;
	payload << roomId;
	stRoom* room = roomList->getRoom(roomId);
	payload << (unsigned short)(room->_nameLen * 2);
	payload.putDataW(room->_nameLen, room->_name);
	payload << room->_userNum;
	for (CUserList::USER_LIST_TYPE::iterator iter = room->userListBegin(); iter != room->userListEnd(); ++iter) {
		payload.putDataW(15,(*iter).second->_name);
		payload << (*iter).second->_id;
	}

	CProtocolBuffer header(6);
	header << (unsigned char)0x89;
	header << network->getCheckSum(&payload, RES_RoomEnter);
	header << (unsigned short)RES_RoomEnter;
	header << (unsigned short)payload.getUsedSize();

	network->uniCast(user->_sendBuffer, header.getUsedSize(), &header);
	network->uniCast(user->_sendBuffer, payload.getUsedSize(), &payload);
	wprintf(L"[SEND] RoomEnter, result: %d, roomId\n", result, roomId);
	return true;
}

bool CStubFunc::RES_ChatStub(stUser* user, unsigned int userId, unsigned short msgSize, wchar_t* msg) {

	CProtocolBuffer payload(50);
	payload << userId;
	payload << (unsigned short)(msgSize * 2);
	payload.putDataW(msgSize, msg);

	CProtocolBuffer header(6);
	header << (unsigned char)0x89;
	header << network->getCheckSum(&payload, RES_Chat);
	header << (unsigned short)RES_Chat;
	header << (unsigned short)payload.getUsedSize();

	unsigned int roomId = user->_roomId;

	stRoom* room = roomList->getRoom(roomId);

	network->broadCast(room->userListBegin(), room->userListEnd(), user, header.getUsedSize(), &header);
	network->broadCast(room->userListBegin(), room->userListEnd(), user, payload.getUsedSize(), &payload);
	wprintf(L"[SEND] Chat, userId: %d\n", userId);
	return true;
}

bool CStubFunc::RES_RoomLeaveStub(stUser* user, unsigned int userId) {

	CProtocolBuffer payload(50);
	payload << userId;

	CProtocolBuffer header(6);
	header << (unsigned char)0x89;
	header << network->getCheckSum(&payload, RES_RoomLeave);
	header << (unsigned short)RES_RoomLeave;
	header << (unsigned short)payload.getUsedSize();

	unsigned int roomId = user->_roomId;

	stRoom* room = roomList->getRoom(roomId);

	network->broadCast(room->userListBegin(), room->userListEnd(), nullptr, header.getUsedSize(), &header);
	network->broadCast(room->userListBegin(), room->userListEnd(), nullptr, payload.getUsedSize(), &payload);
	wprintf(L"[SEND] RoomLeave, userId: %d\n", userId);
	return true;
}

bool CStubFunc::RES_RoomDeleteStub(stUser* user, unsigned int roomId) {

	CProtocolBuffer payload(50);
	payload << roomId;

	CProtocolBuffer header(6);
	header << (unsigned char)0x89;
	header << network->getCheckSum(&payload, RES_RoomDelete);
	header << (unsigned short)RES_RoomDelete;
	header << (unsigned short)payload.getUsedSize();

	network->broadCast(userList->userListBegin(), userList->userListEnd(), nullptr, header.getUsedSize(), &header);
	network->broadCast(userList->userListBegin(), userList->userListEnd(), nullptr, payload.getUsedSize(), &payload);
	wprintf(L"[SEND] RoomDelete, roomId: %d\n", roomId);
	return true;
}

bool CStubFunc::RES_UserEnterStub(stUser* user, unsigned int roomId) {

	CProtocolBuffer payload(50);
	payload.putDataW(15, user->_name);
	payload << user->_id;

	CProtocolBuffer header(6);
	header << (unsigned char)0x89;
	header << network->getCheckSum(&payload, RES_UserEnter);
	header << (unsigned short)RES_UserEnter;
	header << (unsigned short)payload.getUsedSize();

	stRoom* room = roomList->getRoom(roomId);

	network->broadCast(room->userListBegin(), room->userListEnd(), user, header.getUsedSize(), &header);
	network->broadCast(room->userListBegin(), room->userListEnd(), user, payload.getUsedSize(), &payload);
	wprintf(L"[SEND] UserEnter, roomId: %d\n", roomId);

	return true;

}

bool CStubFunc::RES_STRESS_ECHOStub(stUser* user, unsigned short size, char* msg) {

	CProtocolBuffer payload(size + 2);
	payload << (unsigned short)size;
	payload.putData(size, msg);

	CProtocolBuffer header(6);
	header << (unsigned char)0x89;
	header << network->getCheckSum(&payload, RES_STRESS_ECHO);
	header << (unsigned short)RES_STRESS_ECHO;
	header << (unsigned short)payload.getUsedSize();

	network->uniCast(user->_sendBuffer, header.getUsedSize(), &header);
	network->uniCast(user->_sendBuffer, payload.getUsedSize(), &payload);

#ifndef STRESS_TEST
	wprintf(L"[SEND] ECHO, userId: %d, size: %d\n", user->_id, size);
#endif

#ifdef STRESS_TEST
	echoProcCnt += 1;
#endif
	return true;

}