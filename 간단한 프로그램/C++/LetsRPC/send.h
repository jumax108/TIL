class CStubFunc{
	virtual bool SC_CreateMyCharacter(CRingBuffer* sendBuffer, int id, char direction, short x, short y, char hp)
	{
		CProtocolBuffer packet;
		packet << id;
		packet << direction;
		packet << x;
		packet << y;
		packet << hp;
		sendBuffer->push((char*)&packet);
	}
	virtual bool SC_OtherMyCharacter(CRingBuffer* sendBuffer, int id, char direction, short x, short y, char hp)
	{
		CProtocolBuffer packet;
		packet << id;
		packet << direction;
		packet << x;
		packet << y;
		packet << hp;
		sendBuffer->push((char*)&packet);
	}
	virtual bool SC_DeleteCharacter(CRingBuffer* sendBuffer, int id)
	{
		CProtocolBuffer packet;
		packet << id;
		sendBuffer->push((char*)&packet);
	}
	virtual bool CS_MoveStart(CRingBuffer* sendBuffer, char direction, short x, short y)
	{
		CProtocolBuffer packet;
		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push((char*)&packet);
	}
	virtual bool SC_MoveStart(CRingBuffer* sendBuffer, int id, char direction, short x, short y)
	{
		CProtocolBuffer packet;
		packet << id;
		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push((char*)&packet);
	}
	virtual bool CS_MoveStop(CRingBuffer* sendBuffer, char direction, short x, short y)
	{
		CProtocolBuffer packet;
		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push((char*)&packet);
	}
	virtual bool SC_MoveStop(CRingBuffer* sendBuffer, int id, char direction, short x, short y)
	{
		CProtocolBuffer packet;
		packet << id;
		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push((char*)&packet);
	}
	virtual bool CS_Attack1(CRingBuffer* sendBuffer, char direction, short x, short y)
	{
		CProtocolBuffer packet;
		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push((char*)&packet);
	}
	virtual bool SC_Attack1(CRingBuffer* sendBuffer, int id, char direction, short x, short y)
	{
		CProtocolBuffer packet;
		packet << id;
		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push((char*)&packet);
	}
	virtual bool CS_Attack2(CRingBuffer* sendBuffer, char direction, short x, short y)
	{
		CProtocolBuffer packet;
		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push((char*)&packet);
	}
	virtual bool SC_Attack2(CRingBuffer* sendBuffer, int id, char direction, short x, short y)
	{
		CProtocolBuffer packet;
		packet << id;
		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push((char*)&packet);
	}
	virtual bool CS_Attack3(CRingBuffer* sendBuffer, char direction, short x, short y)
	{
		CProtocolBuffer packet;
		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push((char*)&packet);
	}
	virtual bool SC_Attack3(CRingBuffer* sendBuffer, int id, char direction, short x, short y)
	{
		CProtocolBuffer packet;
		packet << id;
		packet << direction;
		packet << x;
		packet << y;
		sendBuffer->push((char*)&packet);
	}
	virtual bool SC_Damage(CRingBuffer* sendBuffer, int attackId, int damageId, char damageHp)
	{
		CProtocolBuffer packet;
		packet << attackId;
		packet << damageId;
		packet << damageHp;
		sendBuffer->push((char*)&packet);
	}
};
