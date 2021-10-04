class CNetwork;
extern CNetwork* network;
class CStubFunc{
	virtual bool REQ_LogInStub(CRingBuffer* sendBuffer, wchar name[15])
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stREQ_LogIn);
		packet<<(char)REQ_LogIn;
		packet << name[15];
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
		if(network->ableSendPacket == true){
			network->sendPacket();
		}
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
		if(network->ableSendPacket == true){
			network->sendPacket();
		}
	}
	virtual bool REQ_RoomListStub(CRingBuffer* sendBuffer)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stREQ_RoomList);
		packet<<(char)REQ_RoomList;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
		if(network->ableSendPacket == true){
			network->sendPacket();
		}
	}
	virtual bool RES_RoomListStub(CRingBuffer* sendBuffer, unsigned short roomNum, unsigned int roomId, unsigned short roomNameLen, wchar_t* roomName, unsigned char userNum, wchar_t** userName)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stRES_RoomList);
		packet<<(char)RES_RoomList;
		packet << roomNum;
		packet << roomId;
		packet << roomNameLen;
		packet << roomName;
		packet << userNum;
		packet << userName;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
		if(network->ableSendPacket == true){
			network->sendPacket();
		}
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
		if(network->ableSendPacket == true){
			network->sendPacket();
		}
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
		if(network->ableSendPacket == true){
			network->sendPacket();
		}
	}
	virtual bool REQ_RoomEnterStub(CRingBuffer* sendBuffer, unsigned int roomId)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stREQ_RoomEnter);
		packet<<(char)REQ_RoomEnter;
		packet << roomId;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
		if(network->ableSendPacket == true){
			network->sendPacket();
		}
	}
	virtual bool RES_RoomEnterStub(CRingBuffer* sendBuffer, unsigned char result, unsigned int roomId, unsigned short roomNameSize, wchar_t* roomName, unsigned char userNum, wchar_t** userName, unsigned int* userId)
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
		packet << userName;
		packet << userId;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
		if(network->ableSendPacket == true){
			network->sendPacket();
		}
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
		if(network->ableSendPacket == true){
			network->sendPacket();
		}
	}
	virtual bool REQ_RoomLeaveStub(CRingBuffer* sendBuffer)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stREQ_RoomLeave);
		packet<<(char)REQ_RoomLeave;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
		if(network->ableSendPacket == true){
			network->sendPacket();
		}
	}
	virtual bool RES_RoomDeleteStub(CRingBuffer* sendBuffer)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stRES_RoomDelete);
		packet<<(char)RES_RoomDelete;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
		if(network->ableSendPacket == true){
			network->sendPacket();
		}
	}
	virtual bool RES_UserEnterStub(CRingBuffer* sendBuffer, wchar_t* name, unsigned int id)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stRES_UserEnter);
		packet<<(char)RES_UserEnter;
		packet << name;
		packet << id;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
		if(network->ableSendPacket == true){
			network->sendPacket();
		}
	}
	virtual bool REQ_STRESS_ECHOStub(CRingBuffer* sendBuffer, short size, wchar_t* str)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stREQ_STRESS_ECHO);
		packet<<(char)REQ_STRESS_ECHO;
		packet << size;
		packet << str;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
		if(network->ableSendPacket == true){
			network->sendPacket();
		}
	}
	virtual bool RES_STRESS_ECHOStub(CRingBuffer* sendBuffer, short size, wchar_t* str)
	{
		CProtocolBuffer packet(50);
		packet<<(char)0x89;
		packet<<(char)sizeof(stRES_STRESS_ECHO);
		packet<<(char)RES_STRESS_ECHO;
		packet << size;
		packet << str;
		sendBuffer->push(packet.getUsedSize(), packet.getFrontPtr());
		if(network->ableSendPacket == true){
			network->sendPacket();
		}
	}
};
