#pragma pack(1)

struct stHeader{
	unsigned char code;
	unsigned char checkSum;

	unsigned short payloadType;
	unsigned short payloadSize;
};

struct stREQ_LogIn{
	wchar_t name[15];
};
struct stRES_LogIn{
	unsigned char result;
	unsigned int userId;
};
struct stREQ_RoomList{
};
struct stRES_RoomList{
	short num;
	stRoom* room;
};
struct stREQ_RoomCreate{
	unsigned short nameSize;
	wchar_t* name;
};
struct stRES_RoomCreate{
	unsigned char result;
	unsigned int roomId.;
	unsigned short roomNameSize;
	wchar_t* roomName;
};
struct stREQ_RoomEnter{
	unsigned int roomId;
};
struct stRES_RoomEnter{
	unsigned char result;
	unsigned int roomId;
	unsigned short roomNameSize;
	wchar_t* roomName;
	unsigned char userNum;
	stUser* user;
};
struct stREQ_Chat{
	unsigned int userId;
	unsigned short msgSize;
	wchar_t* msg;
};
struct stREQ_RoomLeave{
};
struct stRES_RoomDelete{
};
struct stRES_UserEnter{
	wchar_t name[15];
	unsigned int id;
};
#pragma pack(0)
constexpr unsigned int REQ_LogIn = 1;
constexpr unsigned int RES_LogIn = 2;
constexpr unsigned int REQ_RoomList = 3;
constexpr unsigned int RES_RoomList = 4;
constexpr unsigned int REQ_RoomCreate = 5;
constexpr unsigned int RES_RoomCreate = 6;
constexpr unsigned int REQ_RoomEnter = 7;
constexpr unsigned int RES_RoomEnter = 8;
constexpr unsigned int REQ_Chat = 9;
constexpr unsigned int REQ_RoomLeave = 10;
constexpr unsigned int RES_RoomDelete = 11;
constexpr unsigned int RES_UserEnter = 12;
