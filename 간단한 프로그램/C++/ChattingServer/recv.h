class CProxyFuncBase{
public:
	virtual bool REQ_LogInProxy(wchar_t name[15]){ return false; }
	virtual bool RES_LogInProxy(unsigned char result, unsigned int userId){ return false; }
	virtual bool REQ_RoomListProxy(){ return false; }
	virtual bool RES_RoomListProxy(short num, stRoom* room){ return false; }
	virtual bool REQ_RoomCreateProxy(unsigned short nameSize, wchar_t* name){ return false; }
	virtual bool RES_RoomCreateProxy(unsigned char result, unsigned int roomId, unsigned short roomNameSize, wchar_t* roomName){ return false; }
	virtual bool REQ_RoomEnterProxy(unsigned int roomId){ return false; }
	virtual bool RES_RoomEnterProxy(unsigned char result, unsigned int roomId, unsigned short roomNameSize, wchar_t* roomName, unsigned char userNum, stUser* user){ return false; }
	virtual bool REQ_ChatProxy(unsigned int userId, unsigned short msgSize, wchar_t* msg){ return false; }
	virtual bool REQ_RoomLeaveProxy(){ return false; }
	virtual bool RES_RoomDeleteProxy(){ return false; }
	virtual bool RES_UserEnterProxy(wchar_t name[15], unsigned int id){ return false; }
	void packetProc(stHeader* header, CProtocolBuffer* payload, CProxyFuncBase* proxy){
		switch(header->payloadType){
			case REQ_LogIn:
			{
				wchar_t name[15];
				payload->popData(sizeof(wchar_t) * 15, (char*)name);
				proxy->REQ_LogInProxy(name);
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
				short num;
				*payload >> num;
				stRoom* room;
				*payload >> room;
				proxy->RES_RoomListProxy(num, room);
			}
			break;
			case REQ_RoomCreate:
			{
				unsigned short nameSize;
				*payload >> nameSize;
				wchar_t* name = new wchar_t[nameSize];
				payload->popData(nameSize * sizeof(wchar_t), (char*)name);
				proxy->REQ_RoomCreateProxy(nameSize, name);
				delete[] name;
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
				wchar_t* roomName = new wchar_t[roomNameSize];
				payload->popData(roomNameSize * sizeof(wchar_t), (char*)roomName);
				proxy->RES_RoomCreateProxy(result, roomId, roomNameSize, roomName);
				delete[] roomName;
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
				wchar_t* roomName = new wchar_t[roomNameSize];
				payload->popData(roomNameSize * sizeof(wchar_t), (char*)roomName);
				unsigned char userNum;
				*payload >> userNum;
				stUser* user;
				*payload >> user;
				proxy->RES_RoomEnterProxy(result, roomId, roomNameSize, roomName, userNum, user);
				delete[] roomName;
			}
			break;
			case REQ_Chat:
			{
				unsigned int userId;
				*payload >> userId;
				unsigned short msgSize;
				*payload >> msgSize;
				wchar_t* msg = new wchar_t[msgSize];
				payload->popData(sizeof(wchar_t) * msgSize, (char*)msg);
				proxy->REQ_ChatProxy(userId, msgSize, msg);
				delete[] msg;
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
				wchar_t name[15];
				payload->popData(sizeof(wchar_t) * 15, (char*)name);
				unsigned int id;
				*payload >> id;
				proxy->RES_UserEnterProxy(name, id);
			}
			break;
		}
	}
};
