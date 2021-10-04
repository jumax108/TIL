pragma pack(1)
struct stHeader{
	char code;
	unsigned char checkSum;
	char payloadSize;
	char payloadType;
};
struct stREQ_LogIn{
	wchar name[15];
};
struct stRES_LogIn{
	unsigned char result;
	unsigned int userId;
};
struct stREQ_RoomList{
};
struct stRES_RoomList{
	unsigned short roomNum;
	unsigned int roomId;
	unsigned short roomNameLen;
	wchar_t* roomName;
	unsigned char userNum;
	wchar_t** userName;
};
struct stREQ_RoomCreate{
	unsigned short nameSize;
	wchar_t* name;
};
struct stRES_RoomCreate{
	unsigned char result;
	unsigned int roomId;
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
	wchar_t** userName;
	unsigned int* userId;
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
	wchar_t* name;
	unsigned int id;
};
struct stREQ_STRESS_ECHO{
	short size;
	wchar_t* str;
};
struct stRES_STRESS_ECHO{
	short size;
	wchar_t* str;
};
pragma pack(0)
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
constexpr unsigned int REQ_STRESS_ECHO = 100;
constexpr unsigned int RES_STRESS_ECHO = 101;
