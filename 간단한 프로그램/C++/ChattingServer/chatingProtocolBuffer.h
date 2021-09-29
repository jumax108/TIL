#pragma once

class CProtocolBuffer;

class CChatingProtocolBuffer : public CProtocolBuffer {

public:

	CChatingProtocolBuffer* operator<<(stRoom* room) {
		(CProtocolBuffer)(*this) << room->_id;
		(CProtocolBuffer)(*this) << room->_nameLen;
		putData(sizeof(wchar_t) * room->_nameLen, (const char*)room->_name);
		(CProtocolBuffer)(*this) << room->_userNum;
		
		
		for (unordered_map<unsigned int, stUser*>::iterator userIter = userBegin; userIter != userEnd; ++userIter) {
			operator<<(userIter);
		}

		return this;
	}

	CChatingProtocolBuffer* operator<<(stUser* user) {

		putData(sizeof(wchar_t) * 15, (const char*)user->_name);
		CProtocolBuffer::operator<<(user->_id);

		return this;
	}

	CChatingProtocolBuffer* operator>>(stRoom* room) {
		(CProtocolBuffer)(*this) >> room->_id;
		(CProtocolBuffer)(*this) >> room->_nameLen;
		room->_name = new wchar_t[room->_nameLen];
		popData(sizeof(wchar_t) * room->_nameLen, (char*)room->_name);
		(CProtocolBuffer)(*this) >> room->_userNum;



	}

};