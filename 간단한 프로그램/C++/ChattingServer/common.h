#pragma once

struct stHeader {
	unsigned char code;
	unsigned char checkSum;
	unsigned short payloadType;
	unsigned short payloadSize;
};

constexpr unsigned int REQ_LogIn = 1;
constexpr unsigned int RES_LogIn = 2;
constexpr unsigned int REQ_RoomList = 3;
constexpr unsigned int RES_RoomList = 4;
constexpr unsigned int REQ_RoomCreate = 5;
constexpr unsigned int RES_RoomCreate = 6;
constexpr unsigned int REQ_RoomEnter = 7;
constexpr unsigned int RES_RoomEnter = 8;
constexpr unsigned int REQ_Chat = 9;
constexpr unsigned int RES_Chat = 10;
constexpr unsigned int REQ_RoomLeave = 11;
constexpr unsigned int RES_RoomLeave = 12;
constexpr unsigned int RES_RoomDelete = 13;
constexpr unsigned int RES_UserEnter = 14;
constexpr unsigned int REQ_STRESS_ECHO = 100;
constexpr unsigned int RES_STRESS_ECHO = 101;
