#pragma once

#include "../common/packetPtr.h"

class CPacketPtrNet : public CPacketPtr{
	public:
	CPacketPtrNet(){
	
		_packet = _freeList->allocObject();
		_packet->_incoded = false;
		_packet->_ref = 1;

		// 패킷 데이터 초기화
		_packet->_buffer.clear();

		// 패킷 헤더 공간 확보
		_packet->_buffer.moveRear(sizeof(stHeader));

	}

	virtual void incoding(){
		if(_packet->_buffer.getUsedSize() < sizeof(stHeader)){
			CDump::crash();
		}

		if(_packet->_incoded == true){
			return ;
		}
		_packet->_incoded = true;

	
		unsigned char randKey = rand() % 255;

		setHeader(randKey);
	
		int packetSize = _packet->_buffer.getUsedSize();
		char* packetBuf = _packet->_buffer.getBufStart();

		unsigned char beforeEncode1 = 0;
		unsigned char beforeEncode2 = 0;

		for(int packetBufCnt = 4; packetBufCnt < packetSize; ++packetBufCnt){

			unsigned char packetData = packetBuf[packetBufCnt];

			unsigned char encode1 = packetData ^ (beforeEncode1 + randKey + packetBufCnt - 3);
			beforeEncode1 = encode1;

			unsigned char encode2 = encode1 ^ (beforeEncode2 + constKey + packetBufCnt - 3);
			beforeEncode2 = encode2;

			packetBuf[packetBufCnt] = encode2;
		}
	}

	virtual void decoding(){
		if(_packet->_buffer.getUsedSize() < sizeof(stHeader)){
			CDump::crash();
		}

		int packetSize = _packet->_buffer.getUsedSize();
		char* packetBuf = _packet->_buffer.getBufStart();
	
		_packet->_incoded = false;

		unsigned char randKey = packetBuf[3];

		unsigned char beforeEncode1 = 0;
		unsigned char beforeEncode2 = 0;

		for(int packetBufCnt = 4; packetBufCnt < packetSize; ++packetBufCnt){

			unsigned char packetData = packetBuf[packetBufCnt];

			unsigned char encode2 = packetData ^ (beforeEncode2 + constKey + packetBufCnt - 3);
			beforeEncode2 = packetData;

			unsigned char encode1 = encode2 ^ (beforeEncode1 + randKey + packetBufCnt - 3);
			beforeEncode1 = encode2;

			packetBuf[packetBufCnt] = encode1;
		}
	}

	virtual void setHeader(unsigned char randKey){
		int payloadSize = _packet->_buffer.getUsedSize() - sizeof(stHeader);

		stHeader header;
		header.code = 0x77;
		header.randKey = randKey;
		header.size = payloadSize;

		char* payloadBuf = _packet->_buffer.getBufStart() + sizeof(stHeader);

		char checkSum = 0;

		for(int payloadBufCnt = 0; payloadBufCnt < payloadSize; ++ payloadBufCnt){

			checkSum = checkSum + payloadBuf[payloadBufCnt];

		}

		header.checkSum = checkSum;

		memcpy(_packet->_buffer.getBufStart(), &header, sizeof(stHeader));
	}

};