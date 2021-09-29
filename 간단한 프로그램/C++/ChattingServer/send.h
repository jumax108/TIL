class CNetwork;
extern CNetwork* network;

class CStubFunc{
	virtual bool REQ_LogInStub(CRingBuffer* sendBuffer, wchar_t name[15])
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stREQ_LogIn);
		packet<<(char)REQ_LogIn;
		packet << name[15];
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
	}
	virtual bool RES_LogInStub(CRingBuffer* sendBuffer, unsigned char result, unsigned int userId)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stRES_LogIn);
		packet<<(char)RES_LogIn;
		packet << result;
		packet << userId;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());

	}
	virtual bool REQ_RoomListStub(CRingBuffer* sendBuffer)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stREQ_RoomList);
		packet<<(char)REQ_RoomList;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
	}
	virtual bool RES_RoomListStub(CRingBuffer* sendBuffer, short num, stRoom* room)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stRES_RoomList);
		packet<<(char)RES_RoomList;
		packet << num;
		packet << room;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
	}
	virtual bool REQ_RoomCreateStub(CRingBuffer* sendBuffer, unsigned short nameSize, wchar_t* name)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stREQ_RoomCreate);
		packet<<(char)REQ_RoomCreate;
		packet << nameSize;
		packet << name;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());

	}
	virtual bool RES_RoomCreateStub(CRingBuffer* sendBuffer, unsigned char result, unsigned int roomId, unsigned short roomNameSize, wchar_t* roomName)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stRES_RoomCreate);
		packet<<(char)RES_RoomCreate;
		packet << result;
		packet << roomId;
		packet << roomNameSize;
		packet << roomName;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
	
	}
	virtual bool REQ_RoomEnterStub(CRingBuffer* sendBuffer, unsigned int roomId)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stREQ_RoomEnter);
		packet<<(char)REQ_RoomEnter;
		packet << roomId;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
	
	}
	virtual bool RES_RoomEnterStub(CRingBuffer* sendBuffer, unsigned char result, unsigned int roomId, unsigned short roomNameSize, wchar_t* roomName, unsigned char userNum, stUser* user)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stRES_RoomEnter);
		packet<<(char)RES_RoomEnter;
		packet << result;
		packet << roomId;
		packet << roomNameSize;
		packet << roomName;
		packet << userNum;
		packet << user;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());

	}
	virtual bool REQ_ChatStub(CRingBuffer* sendBuffer, unsigned int userId, unsigned short msgSize, wchar_t* msg)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stREQ_Chat);
		packet<<(char)REQ_Chat;
		packet << userId;
		packet << msgSize;
		packet << msg;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
	
	}
	virtual bool REQ_RoomLeaveStub(CRingBuffer* sendBuffer)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stREQ_RoomLeave);
		packet<<(char)REQ_RoomLeave;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
	
	}
	virtual bool RES_RoomDeleteStub(CRingBuffer* sendBuffer)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stRES_RoomDelete);
		packet<<(char)RES_RoomDelete;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
	
	}
	virtual bool RES_UserEnterStub(CRingBuffer* sendBuffer, wchar_t name[15], unsigned int id)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stRES_UserEnter);
		packet<<(char)RES_UserEnter;
		packet << name[15];
		packet << id;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
	
	}
};
