#include "netServer.h"

static constexpr int POST_SEND = 1;

CNetServer::CNetServer(){

	_sessionArr = nullptr;

	_sessionNum = 0;
	_sessionCnt = 0;
	_sessionIndexStack = nullptr;

	_acceptThread = NULL;
	_workerThread = nullptr;

	_sendBufferSize = 0;
	_recvBufferSize = 0;

	_listenSocket = NULL;
	_iocp = NULL;

	_sessionIndexMask     = 0x000000000000FFFF;
	_sessionAllocCntMask  = 0xFFFFFFFFFFFF0000;

	_stopEvent = CreateEvent(nullptr, true, false, L"stopEvent");

	log.setDirectory(L"log");
	log.setPrintGroup(LOG_GROUP::LOG_ERROR | LOG_GROUP::LOG_SYSTEM);

	_acceptTotal = 0;
}

bool CNetServer::disconnect(unsigned __int64 sessionID){
	
	unsigned short sessionIndex = sessionID;//& _sessionIndexMask;
	stSession* session = &_sessionArr[sessionIndex];
	
	session->_disconnect = true;
	
	CancelIoEx((HANDLE)session->_sock, &session->_recvOverlapped);
	CancelIoEx((HANDLE)session->_sock, &session->_sendOverlapped);
	
	// RST 패킷 크기
	InterlockedAdd64((LONG64*)&_sendBytes, PROTOCOL_HEADER_SIZE);

	return true;

}

void CNetServer::release(unsigned __int64 sessionID){

	unsigned short sessionIndex = sessionID;// & _sessionIndexMask;
	stSession* session = &_sessionArr[sessionIndex];
	
	unsigned int* ioCntAndRelease = (unsigned int*)&session->_release;

	if(InterlockedCompareExchange((LONG*)ioCntAndRelease, 1, 0) != 0){
		return ;
	}

	if(session->_sessionID != sessionID){
		printf("before: %I64d session: %I64d\n", sessionID, session->_sessionID);
		session->_release = false;
		recvPost(session);
		return ;
	}

		
	closesocket(session->_sock);

	// recv, send 버퍼 초기화
	session->_recvBuffer.reset();


	CPacketPtr packet;
	CLockFreeQueue<CPacketPtr>* sendQueue = &session->_sendQueue;
	int queSize = sendQueue->getSize();
	while(queSize > 0){
		sendQueue->pop(&packet);
		packet.decRef();
		queSize -= 1;
	}

	int packetNum = session->_packetCnt;
	CPacketPtr* packetIter = session->_packets;
	CPacketPtr* packetEnd = packetIter + packetNum;
	session->_packetCnt = 0;
	for(; packetIter != packetEnd; ++packetIter){
		packetIter->decRef();
	}
	
	if(session->_disconnect == false){
		InterlockedAdd64((LONG64*)&_recvBytes, PROTOCOL_HEADER_SIZE);
	}

	if(_maxSendQueCap < session->_sendQueue.getCapacity()){
		_maxSendQueCap = session->_sendQueue.getCapacity();
	}

	if(_maxRecvBufferCap < session->_recvBuffer.capacity()){
		_maxRecvBufferCap = session->_recvBuffer.capacity();
	}

	unsigned __int64 sessionAllocCnt = (sessionID & _sessionAllocCntMask) >> 16;
		
	onClientLeave(sessionID);

	if(sessionAllocCnt >= 0xFFFFFFFFFFFF){
		log(L"release.txt", LOG_GROUP::LOG_SYSTEM, L"sock: %I64d, id: 0x%I64x, allocCnt: %I64d, sessionPtr: %x%I64x", session->_sock, sessionID, sessionAllocCnt, session);
	} else {
		_sessionIndexStack->push(sessionIndex);
	}
	
	InterlockedDecrement64((LONG64*)&_sessionCnt);

}

bool CNetServer::sendPacketPostIOCP(unsigned __int64 sessionID, CPacketPtrNet packet){
	
	stSession* session = &_sessionArr[(unsigned short)sessionID];

	if(session->_release == true || session->_sessionID != sessionID){
		onError(2, L"no session");
		return false;
	}

	if(session->_disconnect == true){
		return false;
	}

	InterlockedIncrement16(&session->_ioCnt);

	packet.incRef();
	packet.incoding();
	
	session->_sendQueue.push(packet);

	if(session->_isSent == false){
		InterlockedIncrement16((SHORT*)&_iocpQueueSize);
		PostQueuedCompletionStatus(_iocp, 1, (ULONG_PTR)session, (LPOVERLAPPED)POST_SEND);
	}

	if(InterlockedDecrement16(&session->_ioCnt) == 0){

		release(sessionID);
	}

	return true;

}

bool CNetServer::sendPacketPostSend(unsigned __int64 sessionID, CPacketPtrNet packet){
	
	stSession* session = &_sessionArr[(unsigned short)sessionID];

	if(session->_release == true || session->_disconnect == true || session->_sessionID != sessionID){
		onError(2, L"no session");
		return false;
	}

	InterlockedIncrement16(&session->_ioCnt);
	
	packet.incRef();
	packet.incoding();

	session->_sendQueue.push(packet);
	
	bool sendProcessing = InterlockedExchange8((char*)&session->_isSent, true);
	if(sendProcessing == false){
		sendPost(session);
	}
	
	if(InterlockedDecrement16(&session->_ioCnt) == 0){
		release(sessionID);
	}
	return true;

}

unsigned CNetServer::completionStatusFunc(void *args){
	
	//SetThreadPriority((HANDLE)__threadhandle(), THREAD_PRIORITY_HIGHEST);
	CNetServer* server = (CNetServer*)args;

	HANDLE iocp = server->_iocp;
	
	unsigned int transferred;
	stSession* session;
	OVERLAPPED* overlapped;

	for(;;){
		
		GetQueuedCompletionStatus(iocp, (LPDWORD)&transferred, (PULONG_PTR)&session, &overlapped, INFINITE);
		
		unsigned __int64 sessionID = session->_sessionID;
		
		if(&session->_sendOverlapped == overlapped){

			int packetNum = session->_packetCnt;
			CPacketPtr* packets = session->_packets;
			CPacketPtr* packetIter = packets;
			CPacketPtr* packetEnd = packets + packetNum;

			for(; packetIter != packetEnd; ++packetIter){
				packetIter->decRef();
			}

			InterlockedAdd((LONG*)&server->_sendCnt, packetNum);

			session->_isSent = false;
			
			session->_packetCnt = 0;
						
			server->onSend(sessionID, transferred);

			CLockFreeQueue<CPacketPtr>* sendQueue = &session->_sendQueue;
			
			if(sendQueue->getSize() != 0){

				bool sendProcessing = InterlockedExchange8((char*)&session->_isSent, true);
				if(sendProcessing == false){
					server->sendPost(session);
				}
			}

			if(InterlockedDecrement16(&session->_ioCnt) == 0){
				server->release(sessionID);
			}

		} 
		else if(&session->_recvOverlapped == overlapped){
			
			// recv 완료
			CRingBuffer* recvBuffer = &session->_recvBuffer;

			recvBuffer->moveRear(transferred);
			session->_recvPosted = false;

			InterlockedAdd64((LONG64*)&server->_recvBytes, PROTOCOL_HEADER_SIZE * transferred / MSS + transferred);

			// packet proc
			server->checkCompletePacket(session, recvBuffer);

			server->recvPost(session);

			if(InterlockedDecrement16(&session->_ioCnt) == 0){
				server->release(sessionID);
			}
			
		} 
		else if(overlapped == (OVERLAPPED*)POST_SEND){
		
			CLockFreeQueue<CPacketPtr>* sendQueue = &session->_sendQueue;
			
			bool sendProcessing = InterlockedExchange8((char*)&session->_isSent, true);
			if(sendProcessing == false){
				server->sendPost(session);
			}
			

		} 
		else {
			break;
		}
		
		InterlockedDecrement16((SHORT*)&server->_iocpQueueSize);
	}

	return 0;
}

unsigned CNetServer::acceptFunc(void* args){
	
	CNetServer* server = (CNetServer*)args;
	SOCKET listenSocket = server->_listenSocket;

	HANDLE iocp = server->_iocp;

	while(1){

		SOCKADDR_IN addr;
		int addrLen = sizeof(SOCKADDR_IN);
		SOCKET sock = accept(listenSocket, (SOCKADDR*)&addr, &addrLen);
				
		DWORD stopEventResult = WaitForSingleObject(server->_stopEvent, 0);
		if(WAIT_OBJECT_0 == stopEventResult){
			// stop thread
			break;
		}

		unsigned int ip = addr.sin_addr.S_un.S_addr;
		unsigned short port = addr.sin_port;
		
		/////////////////////////////////////////////////////////
		// 서버 접속 가능 체크
		/////////////////////////////////////////////////////////
		if(server->onConnectRequest(ip, port) == false){
			// 접속이 거부되었습니다.
			closesocket(sock);
			continue;
		}
		/////////////////////////////////////////////////////////
		
		/////////////////////////////////////////////////////////
		// 접속 허용
		/////////////////////////////////////////////////////////
		{
			unsigned int sessionNum = server->_sessionNum;
			unsigned int* sessionCnt = &server->_sessionCnt;
			bool isSessionFull = sessionNum == *sessionCnt;
			
			/////////////////////////////////////////////////////////
			// 동접 최대치 체크
			/////////////////////////////////////////////////////////
			if(isSessionFull == true){
				server->onError(1, L"서버에 세팅된 동시접속자 최대치에 도달하여 새로운 연결을 받을 수 없습니다.");
				closesocket(sock);
				continue;
			}
			/////////////////////////////////////////////////////////
			
			/////////////////////////////////////////////////////////
			// 세션 초기화
			/////////////////////////////////////////////////////////
			
			// session array의 index 확보
			unsigned short sessionIndex = 0;
			CLockFreeStack<unsigned short>* sessionIndexStack = server->_sessionIndexStack;
			sessionIndexStack->pop(&sessionIndex);



			// session 확보
			stSession* sessionArr = server->_sessionArr;
			stSession* session = &sessionArr[sessionIndex];

			unsigned __int64 sessionID = session->_sessionID;

			// ID의 상위 6바이트는 세션 메모리에 대한 재사용 횟수
			// 하위 2바이트는 세션 인덱스

			// session id 세팅
			if(sessionID == 0){
				// 세션 최초 사용
				int sendBufferSize = server->_sendBufferSize;
				int recvBufferSize = server->_recvBufferSize;

				new (session) stSession(sendBufferSize, recvBufferSize);

				session->_sessionID = (unsigned __int64)session;

			} else {
				// 세션 재사용
				unsigned __int64 sessionIDMask = server->_sessionAllocCntMask;

				sessionID &= sessionIDMask;

			}
			sessionID = ((sessionID >> 16) + 1) << 16;
			sessionID |= (unsigned __int64)sessionIndex;
			session->_sessionID = sessionID;
			session->_ip = ip;
			session->_port = port;
			session->_isSent = false;
			session->_sock = sock;		
			session->_recvPosted = false;
			session->_disconnect = false;

			CRingBuffer* recvBuffer = &session->_recvBuffer;
			CLockFreeQueue<CPacketPtr>* sendQueue = &session->_sendQueue;

			// recv, send 버퍼 초기화
			session->_recvBuffer.reset();

			CPacketPtr packet;			
			int queSize = sendQueue->getSize();
			while(queSize > 0){
				sendQueue->pop(&packet);
				packet.decRef();
				queSize -= 1;
			}

			int packetNum = session->_packetCnt;
			session->_packetCnt = 0;
			CPacketPtr* packetIter = session->_packets;
			CPacketPtr* packetEnd = packetIter + packetNum;

			for(; packetIter != packetEnd; ++packetIter){
				packetIter->decRef();	
			}

			InterlockedIncrement((LONG*)&server->_sessionCnt);
			server->_acceptCnt += 1;
			server->_acceptTotal += 1;

			CreateIoCompletionPort((HANDLE)sock, iocp, (ULONG_PTR)session, 0);
			
			// connnect 요청에 대한 ack
			InterlockedAdd64((LONG64*)&server->_sendBytes, 56);

			// connect 요청
			InterlockedAdd64((LONG64*)&server->_recvBytes, 100);
			

			InterlockedIncrement16(&session->_ioCnt);
			session->_release = false;
			server->onClientJoin(ip, port, sessionID);
			
			server->recvPost(session);
			InterlockedDecrement16(&session->_ioCnt);
			if(session->_ioCnt == 0){
				server->release(session->_sessionID);
			}

			/////////////////////////////////////////////////////////

		}
		/////////////////////////////////////////////////////////
	}

	return 0;
}

void CNetServer::recvPost(stSession* session){
	
	unsigned __int64 sessionID = session->_sessionID;

	short* ioCnt = &session->_ioCnt;
	InterlockedIncrement16(ioCnt);
	
	/////////////////////////////////////////////////////////
	// 이미 recv가 걸려있는 상황이 아닌지 체크
	/////////////////////////////////////////////////////////
	bool recvPosted = InterlockedExchange8((CHAR*)&session->_recvPosted, 1);
	if(recvPosted == true){
		InterlockedDecrement16(ioCnt);
		if(*ioCnt == 0){
			release(sessionID);
		}
		return ;
	}
	/////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////
	// 세션이 release 되는 상황이 아닌지 체크	
	/////////////////////////////////////////////////////////
	if(session->_release == true){
		InterlockedDecrement16(ioCnt);
		if(*ioCnt == 0){
			release(sessionID);
		}
		return ;
	}
	/////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////
	// 세션이 disconnect 되는 상황이 아닌지 체크	
	/////////////////////////////////////////////////////////
	if(session->_disconnect == true){
		InterlockedDecrement16(ioCnt);
		if(*ioCnt == 0){
			release(sessionID);
		}
		return ;
	}
	/////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////
	// recv buffer 정보를 wsa buf로 복사
	/////////////////////////////////////////////////////////
	WSABUF wsaBuf[2];
	int wsaCnt = 1;

	CRingBuffer* recvBuffer = &session->_recvBuffer;

	int rear = recvBuffer->rear();
	int front = recvBuffer->front();
	char* directPushPtr = recvBuffer->getDirectPush();
	int directFreeSize = recvBuffer->getDirectFreeSize();
	char* bufStartPtr = recvBuffer->getBufferStart();

	wsaBuf[0].buf = directPushPtr;
	wsaBuf[0].len = directFreeSize;

	if(front <= rear){
		wsaBuf[1].buf = bufStartPtr;
		wsaBuf[1].len = front;
		wsaCnt = 2;
	}
	/////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////
	// wsa recv
	/////////////////////////////////////////////////////////
	OVERLAPPED* overlapped = &session->_recvOverlapped;
	SOCKET sock = session->_sock;
	
	int recvResult;
	int recvError;

	unsigned int flag = 0;
	recvResult = WSARecv(sock, wsaBuf, wsaCnt, nullptr, (LPDWORD)&flag, overlapped, nullptr);
	if(recvResult == SOCKET_ERROR){
		recvError = WSAGetLastError();
		if(recvError != WSA_IO_PENDING){
			InterlockedDecrement16(ioCnt);
			if(*ioCnt == 0){
				release(sessionID);
			}
			if(recvError != 10054){
				log(L"recv.txt", LOG_GROUP::LOG_DEBUG, L"session: 0x%I64x, sock: %I64d, wsaCnt: %d, wsaBuf[0]: 0x%I64x, wsaBuf[1]: 0x%I64x\n", sessionID, sock, wsaCnt, wsaBuf[0], wsaBuf[1]);
			}
					
		} else {
			InterlockedIncrement16((SHORT*)&_iocpQueueSize);
		}
	} else {
		InterlockedIncrement16((SHORT*)&_iocpQueueSize);
	}
	/////////////////////////////////////////////////////////
//	sp.profileEnd("RECV POST");
}

void CNetServer::sendPost(stSession* session){
	
	unsigned __int64 sessionID = session->_sessionID;

	short* ioCnt = &session->_ioCnt;
	InterlockedIncrement16(ioCnt);
	
	/////////////////////////////////////////////////////////
	// 세션이 release 되는 상황이 아닌지 체크
	/////////////////////////////////////////////////////////
	if(session->_release == true){
		session->_isSent = false;
		InterlockedDecrement16(ioCnt);
		if(*ioCnt == 0){
			release(sessionID);
		}
		return ;
	}
	/////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////
	// 세션이 disconnect 되는 상황이 아닌지 체크
	/////////////////////////////////////////////////////////
	if(session->_disconnect == true){
		session->_isSent = false;
		InterlockedDecrement16(ioCnt);
		if(*ioCnt == 0){
			release(sessionID);
		}
		return ;
	}
	/////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////
	// 보낼 데이터가 있는지 체크
	/////////////////////////////////////////////////////////
	CLockFreeQueue<CPacketPtr>* sendQueue = &session->_sendQueue;
	int wsaNum;

	wsaNum = sendQueue->getSize();

	if(wsaNum == 0){
	
		session->_isSent = false;
		InterlockedDecrement16(ioCnt);
		if(*ioCnt == 0){
			release(sessionID);
		}
		return ;
	} else if(wsaNum > MAX_PACKET){
		wsaNum = MAX_PACKET;
	}

	//wsaNum = min(wsaNum, MAX_PACKET);
	
	/////////////////////////////////////////////////////////
		
	/////////////////////////////////////////////////////////
	// packet을 wsaBuf로 복사
	/////////////////////////////////////////////////////////
	WSABUF wsaBuf[MAX_PACKET];
	session->_packetCnt = wsaNum;

	CPacketPtr packet;

	WSABUF* wsaIter = wsaBuf;
	CPacketPtr* packetIter = session->_packets;
	CPacketPtr* packetEnd = packetIter + wsaNum;

	unsigned __int64 sendSize = 0;

	for(; packetEnd != packetIter ;++packetIter){
		
		sendQueue->pop(&packet);
		wsaIter->buf = packet.getBufStart();
		int packetSize = packet.getPacketSize();
		wsaIter->len = packetSize;
		sendSize += packetSize;
		++wsaIter;
		*packetIter = packet;
		
	}

	//memcpy(&packet, packetPtr, sizeof(CPacketPtr));
	
	/////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////
	// wsa send
	/////////////////////////////////////////////////////////
	int sendResult;
	int sendError;

	SOCKET sock = session->_sock;
	OVERLAPPED* overlapped = &session->_sendOverlapped;

	InterlockedAdd64((LONG64*)&_sendBytes, PROTOCOL_HEADER_SIZE * sendSize / MSS + sendSize);

	sendResult = WSASend(sock, wsaBuf, wsaNum, nullptr, 0, overlapped, nullptr);

	if(sendResult == SOCKET_ERROR){
		sendError = WSAGetLastError();
		if(sendError != WSA_IO_PENDING){
			InterlockedDecrement16(ioCnt);
			if(*ioCnt == 0){
				release(sessionID);
			}
			if(sendError != 10054){
				log(L"send.txt", LOG_GROUP::LOG_SYSTEM, L"session: 0x%I64x, sock: %I64d, wsaNum: %d, wsaBuf[0]: 0x%I64x, wsaBuf[1]: 0x%I64x, error: %d\n", sessionID, sock, wsaNum, wsaBuf[0], wsaBuf[1], sendError);
			}
			return ;
		} 
	}
	/////////////////////////////////////////////////////////
	
	InterlockedIncrement16((SHORT*)&_iocpQueueSize);
}

void CNetServer::start(const wchar_t* configFileName){

	CStringParser settingParser(configFileName);

	// 힙 생성
	_heap = HeapCreate(0, 0, 0);

	// 세팅 파일에서 필요한 데이터 수집
	wchar_t* serverIP;
	unsigned short serverPort;
	int createWorkerThreadNum;
	int runningWorkerThreadNum;
	{
		settingParser.setNameSpace(L"lanServer");

		wchar_t buf[20];

		ZeroMemory(buf, 20);
		settingParser.getValueByKey(buf, L"serverIP");
		size_t len = wcslen(buf);
		serverIP = new wchar_t[len + 1];
		serverIP[len] = '\0';
		wmemcpy(serverIP, buf, len);
		
		ZeroMemory(buf, 20);
		settingParser.getValueByKey(buf, L"serverPort");
		serverPort = (unsigned short)wcstol(buf, nullptr, 10);

		ZeroMemory(buf, 20);
		settingParser.getValueByKey(buf, L"createWorkerThreadNum");
		createWorkerThreadNum = wcstol(buf, nullptr, 10);

		ZeroMemory(buf, 20);
		settingParser.getValueByKey(buf, L"runningWorkerThreadNum");
		runningWorkerThreadNum = wcstol(buf, nullptr, 10);

		ZeroMemory(buf, 20);
		settingParser.getValueByKey(buf, L"maxSessionNum");
		_sessionNum = wcstol(buf, nullptr, 10);

		ZeroMemory(buf, 20);
		settingParser.getValueByKey(buf, L"sessionSendBufferSize");
		_sendBufferSize = wcstol(buf, nullptr, 10);

		ZeroMemory(buf, 20);
		settingParser.getValueByKey(buf, L"sessionRecvBufferSize");
		_recvBufferSize = wcstol(buf, nullptr, 10);

	}

	// wsa startup
	int startupResult;
	int startupError;
	{
		WSAData wsaData;
		startupResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if(startupResult != NULL){
			startupError = WSAGetLastError();
			stStartError error(startupError, L"wsa startup Error");
			throw error;
		}
	}

	// 리슨 소켓 생성
	int socketError;
	{
		_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
		if(_listenSocket == INVALID_SOCKET){
			socketError = WSAGetLastError();
			stStartError error(socketError, L"listen socket init Error");
			throw error;
		}
	}

	// 리슨 소켓에 linger 옵션 적용
	int lingerResult;
	int lingerError;
	{
		LINGER lingerSet = {(unsigned short)1, (unsigned short)0};
		lingerResult = setsockopt(_listenSocket, SOL_SOCKET, SO_LINGER, (const char*)&lingerSet, sizeof(LINGER));
		if(lingerResult == SOCKET_ERROR){
			lingerError = WSAGetLastError();
			stStartError error(lingerError, L"linger Error");
			throw error;
		}
	}

	// 리슨 소켓에 노딜레이 옵션 적용
	int noDelayResult;
	int noDelayError;
	{
		bool noDelay = true;
		noDelayResult = setsockopt(_listenSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&noDelay, sizeof(bool));
		if(noDelayResult == SOCKET_ERROR){
			noDelayError = WSAGetLastError();
			stStartError error(noDelayError, L"linger Error");
			throw error;
		}
	}

	// 리슨 소켓 send buffer 0
	int setSendBufferResult;
	int setSendBufferError;
	{
		int sendBufferSize = 0;
		setSendBufferResult = setsockopt(_listenSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufferSize, sizeof(int));
		if(setSendBufferResult == SOCKET_ERROR){

			setSendBufferError = WSAGetLastError();
			stStartError error(setSendBufferError, L"set sendBuffer Error");
			throw error;

		}
	}
	
	// 리슨 소켓 바인딩
	int bindResult;
	int bindError;
	{
		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		InetPtonW(AF_INET, serverIP, &addr.sin_addr.S_un.S_addr);
		addr.sin_port = htons(serverPort);

		bindResult = bind(_listenSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
		if(bindResult == SOCKET_ERROR){
			bindError = WSAGetLastError();
			stStartError error(bindError, L"bind Error");
			throw error;
		}

	}

	// 리스닝 시작
	int listenResult;
	int listenError;
	{
		listenResult = listen(_listenSocket, SOMAXCONN);
		if(listenResult == SOCKET_ERROR){
			listenError = WSAGetLastError();
			stStartError error(listenError, L"listen Error");
			throw error;
		}
	}

	// session 배열 초기화
	{
		_sessionArr = (stSession*)malloc(sizeof(stSession) * _sessionNum);
		ZeroMemory(_sessionArr, sizeof(stSession) * _sessionNum);
		_sessionIndexStack = new CLockFreeStack<unsigned short>();
		for(int sessionCnt = _sessionNum - 1; sessionCnt >= 0 ; --sessionCnt){
			_sessionIndexStack->push(sessionCnt);
		}
	}


	// iocp 초기화
	int iocpError;
	{
		_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, runningWorkerThreadNum);
		if(_iocp == NULL){
			iocpError = GetLastError();
			stStartError error(iocpError, L"create io completion port Error");
			throw error;
		}
	}

	// worker thread 초기화
	{
		_workerThreadNum = createWorkerThreadNum;
		_workerThread = new HANDLE[createWorkerThreadNum];

		for(int threadCnt = 0; threadCnt < createWorkerThreadNum ; ++threadCnt){
			_workerThread[threadCnt] = (HANDLE)_beginthreadex(nullptr, 0, CNetServer::completionStatusFunc, (void*)this, 0, nullptr);
		}

	}

	// accept thread 초기화
	{
		_acceptThread = (HANDLE)_beginthreadex(nullptr, 0, CNetServer::acceptFunc, (void*)this, 0, nullptr);
	}

	// tps thread 초기화
	{
		_tpsThread = (HANDLE)_beginthreadex(nullptr, 0, CNetServer::tpsFunc, (void*)this, 0, nullptr);
	}

}

unsigned __int64 CNetServer::getSessionCount(){
	
	return _sessionCnt;
	
}

void CNetServer::checkCompletePacket(stSession* session, CRingBuffer* recvBuffer){
	
	unsigned __int64 sessionID = session->_sessionID;
	unsigned int usedSize = recvBuffer->getUsedSize();

	while(usedSize > sizeof(stHeader)){
		
		if(session->_release == true){
			return ;
		}

		// header 체크
		stHeader header;
		recvBuffer->front(sizeof(stHeader), (char*)&header);

		if(header.code != 0x77){
			CDump::crash();
		}

		int payloadSize = header.size;
		int packetSize = payloadSize + sizeof(stHeader);

		if(usedSize < packetSize){
			return ;
		}

		// 패킷 완성			
		recvBuffer->pop(sizeof(stHeader));

		CPacketPtrNet packet;

		stHeader* packetHeader = (stHeader*)packet.getBufStart();
		packetHeader->randKey = header.randKey;
		packetHeader->checkSum = header.checkSum;

		recvBuffer->front(payloadSize, packet.getRearPtr());
		recvBuffer->pop(payloadSize);
		packet.moveRear(payloadSize);

		packet.decoding();
		packet.moveFront(sizeof(stHeader));
			
		onRecv(sessionID, packet);
		packet.decRef();

		InterlockedIncrement((LONG*)&_recvCnt);
			
		usedSize -= packetSize;

	}
}

void CNetServer::stop(){

	printf("start stop\n");

	sp.printToFile();

	closesocket(_listenSocket);

	bool setStopEventResult = SetEvent(_stopEvent);
	int setEventError;
	if(setStopEventResult == false){
		setEventError = GetLastError();
		CDump::crash();
	}

	WaitForSingleObject(_acceptThread, INFINITE);

	for(int sessionCnt = 0; sessionCnt < _sessionNum ; ++sessionCnt){
		disconnect(_sessionArr[sessionCnt]._sessionID);
	}
	while(_sessionCnt > 0);

	for(int threadCnt = 0 ; threadCnt < _workerThreadNum; ++threadCnt){
		PostQueuedCompletionStatus((HANDLE)_iocp, (DWORD)0, (ULONG_PTR)0, (LPOVERLAPPED)0);
	}
	WaitForMultipleObjects(_workerThreadNum, _workerThread, true, INFINITE);

	CloseHandle(_iocp);
	
	delete _sessionIndexStack;
	delete[] _sessionArr;
	delete[] _workerThread;
}

unsigned __stdcall CNetServer::tpsFunc(void* args){

	CNetServer* server = (CNetServer*)args;

	for(;;){

		DWORD waitResult = WaitForSingleObject(server->_stopEvent, 0);
		
		if(waitResult == WAIT_OBJECT_0){
			break;
		}

		server->_sendTPS   = server->_sendCnt;
		server->_recvTPS   = server->_recvCnt;
		server->_acceptTPS = server->_acceptCnt;

		server->_sendCnt   = 0;
		server->_recvCnt   = 0;
		server->_acceptCnt = 0;

		Sleep(999);

	}

	return 0;

}

unsigned __int64 CNetServer::getRecvTPS(){
	return _recvTPS;
}
unsigned __int64 CNetServer::getSendTPS(){
	return _sendTPS;
}
unsigned __int64 CNetServer::getAcceptTPS(){
	return _acceptTPS;
}
unsigned __int64 CNetServer::getAcceptTotal(){
	return _acceptTotal;
}