#pragma once

constexpr unsigned short REQ_LogIn = 1;
constexpr unsigned short RES_LogIn = 2;
constexpr unsigned short REQ_Sector_Move = 3;
constexpr unsigned short RES_Sector_Move = 4;
constexpr unsigned short REQ_Chat_Message = 5;
constexpr unsigned short RES_Chat_Message = 6;

#pragma pack(1)
struct stREQ_LogIn{
	unsigned short type;
	__int64 accountNo;
	wchar_t ID[20];
	wchar_t nickName[20];
	char sessionKey[64];
};
struct stRES_LogIn{
	unsigned short type;
	unsigned char status;
	__int64 accountNo;
};
struct stREQ_Sector_Move{
	unsigned short type;
	__int64 accountNo;
	unsigned short sectorX;
	unsigned short sectorY;
};
struct stRES_Sector_Move{
	unsigned short type;
	__int64 accountNo;
	unsigned short sectorX;
	unsigned short sectorY;
};
struct stREQ_Chat_Message{
	unsigned short type;
	__int64 accountNo;
	unsigned short messageLen;
	wchar_t* message;
};
struct stRES_Chat_Message{
	unsigned short type;
	__int64 accountNo;
	wchar_t id[20];
	wchar_t nickName[20];
	unsigned short messageLen;
	wchar_t* message;
};
#pragma pack()
