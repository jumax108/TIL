class CProxyFuncBase{
public:
	virtual bool REQ_LogInProxy(wchar name[15]){ return false; }
	virtual bool RES_LogInProxy(unsigned char result, unsigned int userId){ return false; }
	virtual bool REQ_RoomListProxy(){ return false; }
	virtual bool RES_RoomListProxy(unsigned short roomNum, unsigned int roomId, unsigned short roomNameLen, wchar_t* roomName, unsigned char userNum, wchar_t** userName){ return false; }
	virtual bool REQ_RoomCreateProxy(unsigned short nameSize, wchar_t* name){ return false; }
	virtual bool RES_RoomCreateProxy(unsigned char result, unsigned int roomId, unsigned short roomNameSize, wchar_t* roomName){ return false; }
	virtual bool REQ_RoomEnterProxy(unsigned int roomId){ return false; }
	virtual bool RES_RoomEnterProxy(unsigned char result, unsigned int roomId, unsigned short roomNameSize, wchar_t* roomName, unsigned char userNum, wchar_t** userName, unsigned int* userId){ return false; }
	virtual bool REQ_ChatProxy(unsigned int userId, unsigned short msgSize, wchar_t* msg){ return false; }
	virtual bool REQ_RoomLeaveProxy(){ return false; }
	virtual bool RES_RoomDeleteProxy(){ return false; }
	virtual bool RES_UserEnterProxy(wchar_t* name, unsigned int id){ return false; }
	virtual bool REQ_STRESS_ECHOProxy(short size, wchar_t* str){ return false; }
	virtual bool RES_STRESS_ECHOProxy(short size, wchar_t* str){ return false; }
	void packetProc(stHeader* header, CProtocolBuffer* payload, CProxyFuncBase* proxy){
		switch(header->payloadType){
			case REQ_LogIn:
			{
				wchar name[15];
				*payload >> name[15];
				proxy->REQ_LogInProxy(name[15]);
			}
			break;
			case RES_LogIn:
			{
				unsigned char result;
				*payload >> result;
				unsigned int userId;
				*payload >> userId;
				proxy->RES_LogInProxy(result, userId);
			}
			break;
			case REQ_RoomList:
			{
				proxy->REQ_RoomListProxy();
			}
			break;
			case RES_RoomList:
			{
				unsigned short roomNum;
				*payload >> roomNum;
				unsigned int roomId;
				*payload >> roomId;
				unsigned short roomNameLen;
				*payload >> roomNameLen;
				wchar_t* roomName;
				*payload >> roomName;
				unsigned char userNum;
				*payload >> userNum;
				wchar_t** userName;
				*payload >> userName;
				proxy->RES_RoomListProxy(roomNum, roomId, roomNameLen, roomName, userNum, userName);
			}
			break;
			case REQ_RoomCreate:
			{
				unsigned short nameSize;
				*payload >> nameSize;
				wchar_t* name;
				*payload >> name;
				proxy->REQ_RoomCreateProxy(nameSize, name);
			}
			break;
			case RES_RoomCreate:
			{
				unsigned char result;
				*payload >> result;
				unsigned int roomId;
				*payload >> roomId;
				unsigned short roomNameSize;
				*payload >> roomNameSize;
				wchar_t* roomName;
				*payload >> roomName;
				proxy->RES_RoomCreateProxy(result, roomId, roomNameSize, roomName);
			}
			break;
			case REQ_RoomEnter:
			{
				unsigned int roomId;
				*payload >> roomId;
				proxy->REQ_RoomEnterProxy(roomId);
			}
			break;
			case RES_RoomEnter:
			{
				unsigned char result;
				*payload >> result;
				unsigned int roomId;
				*payload >> roomId;
				unsigned short roomNameSize;
				*payload >> roomNameSize;
				wchar_t* roomName;
				*payload >> roomName;
				unsigned char userNum;
				*payload >> userNum;
				wchar_t** userName;
				*payload >> userName;
				unsigned int* userId;
				*payload >> userId;
				proxy->RES_RoomEnterProxy(result, roomId, roomNameSize, roomName, userNum, userName, userId);
			}
			break;
			case REQ_Chat:
			{
				unsigned int userId;
				*payload >> userId;
				unsigned short msgSize;
				*payload >> msgSize;
				wchar_t* msg;
				*payload >> msg;
				proxy->REQ_ChatProxy(userId, msgSize, msg);
			}
			break;
			case REQ_RoomLeave:
			{
				proxy->REQ_RoomLeaveProxy();
			}
			break;
			case RES_RoomDelete:
			{
				proxy->RES_RoomDeleteProxy();
			}
			break;
			case RES_UserEnter:
			{
				wchar_t* name;
				*payload >> name;
				unsigned int id;
				*payload >> id;
				proxy->RES_UserEnterProxy(name, id);
			}
			break;
			case REQ_STRESS_ECHO:
			{
				short size;
				*payload >> size;
				wchar_t* str;
				*payload >> str;
				proxy->REQ_STRESS_ECHOProxy(size, str);
			}
			break;
			case RES_STRESS_ECHO:
			{
				short size;
				*payload >> size;
				wchar_t* str;
				*payload >> str;
				proxy->RES_STRESS_ECHOProxy(size, str);
			}
			break;
		}
	}
};
