class CProxyFunc{
	virtual bool SC_CreateMyCharacter(int id, char direction, short x, short y, char hp){ return false; }
	virtual bool SC_OtherMyCharacter(int id, char direction, short x, short y, char hp){ return false; }
	virtual bool SC_DeleteCharacter(int id){ return false; }
	virtual bool CS_MoveStart(char direction, short x, short y){ return false; }
	virtual bool SC_MoveStart(int id, char direction, short x, short y){ return false; }
	virtual bool CS_MoveStop(char direction, short x, short y){ return false; }
	virtual bool SC_MoveStop(int id, char direction, short x, short y){ return false; }
	virtual bool CS_Attack1(char direction, short x, short y){ return false; }
	virtual bool SC_Attack1(int id, char direction, short x, short y){ return false; }
	virtual bool CS_Attack2(char direction, short x, short y){ return false; }
	virtual bool SC_Attack2(int id, char direction, short x, short y){ return false; }
	virtual bool CS_Attack3(char direction, short x, short y){ return false; }
	virtual bool SC_Attack3(int id, char direction, short x, short y){ return false; }
	virtual bool SC_Damage(int attackId, int damageId, char damageHp){ return false; }
};
CProxyFunc* proxyFunc = new CProxyFunc();
void packetProc(stHeader* header, CProtocolBuffer* payload){
	switch(header->payloadType){
		case SC_CreateMyCharacter:
		{
			int id;
			*payload >> id;
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			char hp;
			*payload >> hp;
			proxyFunc->SC_CreateMyCharacter(id, direction, x, y, hp);
		}
		break;
		case SC_OtherMyCharacter:
		{
			int id;
			*payload >> id;
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			char hp;
			*payload >> hp;
			proxyFunc->SC_OtherMyCharacter(id, direction, x, y, hp);
		}
		break;
		case SC_DeleteCharacter:
		{
			int id;
			*payload >> id;
			proxyFunc->SC_DeleteCharacter(id);
		}
		break;
		case CS_MoveStart:
		{
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxyFunc->CS_MoveStart(direction, x, y);
		}
		break;
		case SC_MoveStart:
		{
			int id;
			*payload >> id;
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxyFunc->SC_MoveStart(id, direction, x, y);
		}
		break;
		case CS_MoveStop:
		{
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxyFunc->CS_MoveStop(direction, x, y);
		}
		break;
		case SC_MoveStop:
		{
			int id;
			*payload >> id;
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxyFunc->SC_MoveStop(id, direction, x, y);
		}
		break;
		case CS_Attack1:
		{
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxyFunc->CS_Attack1(direction, x, y);
		}
		break;
		case SC_Attack1:
		{
			int id;
			*payload >> id;
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxyFunc->SC_Attack1(id, direction, x, y);
		}
		break;
		case CS_Attack2:
		{
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxyFunc->CS_Attack2(direction, x, y);
		}
		break;
		case SC_Attack2:
		{
			int id;
			*payload >> id;
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxyFunc->SC_Attack2(id, direction, x, y);
		}
		break;
		case CS_Attack3:
		{
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxyFunc->CS_Attack3(direction, x, y);
		}
		break;
		case SC_Attack3:
		{
			int id;
			*payload >> id;
			char direction;
			*payload >> direction;
			short x;
			*payload >> x;
			short y;
			*payload >> y;
			proxyFunc->SC_Attack3(id, direction, x, y);
		}
		break;
		case SC_Damage:
		{
			int attackId;
			*payload >> attackId;
			int damageId;
			*payload >> damageId;
			char damageHp;
			*payload >> damageHp;
			proxyFunc->SC_Damage(attackId, damageId, damageHp);
		}
		break;
	}
}
void recvFunc(SOCKET sock, CRingBuffer* recvBuf){
	while(1){
		unsigned int readSize = recvBuffer.getDirectFreeSize();
		int recvResult = recv(sock, recvBuffer.getRearPtr(), readSize, 0);
		if(recvResult == SOCKET_ERROR){
			int recvError = WSAGetLastError();
			if(recvError == WSAEWOULDBLOCK){
				break;
			}
			return ;
		}
		recvBuffer.moveRear(recvResult);
	}
	unsigned int usedSize
	recvBuf->getUsedSize(&usedSize);
	while(usedSize >= sizeof(stHeader)){
		stHeader header;
		recvBuf->front(sizeof(header), (char*)&header);
		if(usedSize < sizeof(stHeader) + header.payloadSize){
			return ;
		}
		recvBuf->pop(sizeof(header));
		CProtocolBuffer protocolBuffer(sizeof(header));
		recvBuffer->front(header.size, protocolBuffer.getRearPtr());
		protocolBuffer.moveRear(header.size);
		makePacketProc(&header, &protocolBuffer);
		recvBuf->getUsedSize(&usedSize);
	}
}
