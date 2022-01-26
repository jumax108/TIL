#include "lanServer.h"

using namespace MyLanServer;

CLanServer::CLanServer(){

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
}

bool CLanServer::disconnect(unsigned __int64 sessionID){
	
	unsigned short sessionIndex = sessionID & _sessionIndexMask;
	stSession* session = &_sessionArr[sessionIndex];

	CancelIoEx(_iocp, &session->_recvOverlapped);
	CancelIoEx(_iocp, &session->_sendOverlapped);

	return true;

}

void CLanServer::release(unsigned __int64 sessionID){

	unsigned short sessionIndex = sessionID & _sessionIndexMask;
	stSession* session = &_sessionArr[sessionIndex];
	
	unsigned int* ioCntAndRelease = (unsigned int*)&session->_beRelease;
	
	if(InterlockedCompareExchange(ioCntAndRelease, 1, 0) != 0){
		return ;
	}

	if(session->_sessionID != sessionID){
		printf("before: %I64d session: %I64d\n", sessionID, session->_sessionID);
		session->_beRelease = false;
		recvPost(session);
		return ;
	}

	unsigned __int64 sessionAllocCnt = (sessionID & _sessionAllocCntMask) >> 16;

	// send buffer �ȿ� �ִ� ��Ŷ�� delete
	CLockFreeQueue<CPacketPtr>* sendBuffer = &session->_sendQueue;
	
	CPacketPtr packetPtr;
	while(sendBuffer->getSize() > 0){

		sendBuffer->pop(&packetPtr);
		packetPtr.decRef();

	}

	closesocket(session->_sock);
	InterlockedDecrement(&_sessionCnt);
	
	if(sessionAllocCnt >= 0xFFFFFFFFFFFF){
		log(L"release.txt", LOG_GROUP::LOG_SYSTEM, L"sock: %I64d, id: 0x%I64x, allocCnt: %I64d, sessionPtr: %x%I64x", session->_sock, sessionID, sessionAllocCnt, session);
	} else {
		_sessionIndexStack->push(sessionIndex);
	}
	
	InterlockedDecrement64((LONG64*)&_sessionCnt);

}

bool CLanServer::sendPacket(unsigned __int64 sessionID, CPacketPtrLan packet){
	
	unsigned short sessionIndex = sessionID & _sessionIndexMask;
	stSession* session = &_sessionArr[sessionIndex];

	if(session->_beRelease == true || session->_sessionID != sessionID){
		onError(2, L"no session");
		return false;
	}

	InterlockedIncrement16(&session->_ioCnt);
	
	packet.incRef();
	packet.setHeader();

	CLockFreeQueue<CPacketPtr>* sendQueue = &session->_sendQueue;
	sendQueue->push(packet);
	
	bool sendProcessing = InterlockedExchange8((char*)&session->_isSent, true);
	if(sendProcessing == false){
		sendPost(session);
	}

	InterlockedDecrement16(&session->_ioCnt);
	if(session->_ioCnt == 0){
		release(sessionID);
	}
	return true;

}

unsigned CLanServer::completionStatusFunc(void *args){
	
	CLanServer* server = (CLanServer*)args;

	HANDLE iocp = server->_iocp;

	while(1){
		
		unsigned int transferred;
		stSession* session;
		OVERLAPPED* overlapped;
		GetQueuedCompletionStatus(iocp, (LPDWORD)&transferred, (PULONG_PTR)&session, &overlapped, INFINITE);
		
		if(overlapped == nullptr){
			printf("overlapped nullptr\n");
			break;			
		}
			
		unsigned __int64 sessionID = session->_sessionID;
		SOCKET sock = session->_sock;

		if(&session->_sendOverlapped == overlapped){
		
			int packetNum = session->_packetCnt;
			CPacketPtr* packets = session->_packets;
			CPacketPtr* packetIter = packets;
			CPacketPtr* packetEnd = packets + packetNum;

			for(; packetIter != packetEnd; ++packetIter){
				packetIter->decRef();
			}
			
			session->_packetCnt = 0;
			
			InterlockedExchange8((char*)&session->_isSent, false);
			
			CLockFreeQueue<CPacketPtr>* sendQueue = &session->_sendQueue;
			
			if(sendQueue->getSize() != 0){

				bool sendProcessing = InterlockedExchange8((char*)&session->_isSent, true);
				if(sendProcessing == false){
					server->sendPost(session);
				}
			}
			
		}

		if(&session->_recvOverlapped == overlapped){

			// recv �Ϸ�
			CRingBuffer* recvBuffer = &session->_recvBuffer;

			recvBuffer->moveRear(transferred);
			InterlockedExchange8((CHAR*)&session->_recvPosted, 0);

			// packet proc
			server->checkCompletePacket(session, recvBuffer);

			server->recvPost(session);
			
		}

		short* ioCnt = &session->_ioCnt;
		InterlockedDecrement16(ioCnt);
		
		if(*ioCnt == 0){
			server->release(sessionID);
		}

	}

	return 0;
}

unsigned CLanServer::acceptFunc(void* args){
	
	CLanServer* server = (CLanServer*)args;
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
		// ���� ���� ���� üũ
		/////////////////////////////////////////////////////////
		if(server->onConnectRequest(ip, port) == false){
			// ������ �źεǾ����ϴ�.
			closesocket(sock);
			continue;
		}
		/////////////////////////////////////////////////////////
		
		/////////////////////////////////////////////////////////
		// ���� ���
		/////////////////////////////////////////////////////////
		{
			unsigned __int64 sessionNum = server->_sessionNum;
			unsigned __int64* sessionCnt = &server->_sessionCnt;
			bool isSessionFull = sessionNum == *sessionCnt;
			
			/////////////////////////////////////////////////////////
			// ���� �ִ�ġ üũ
			/////////////////////////////////////////////////////////
			if(isSessionFull == true){
				server->onError(1, L"������ ���õ� ���������� �ִ�ġ�� �����Ͽ� ���ο� ������ ���� �� �����ϴ�.");
				closesocket(sock);
				continue;
			}
			/////////////////////////////////////////////////////////
			
			/////////////////////////////////////////////////////////
			// ���� �ʱ�ȭ
			/////////////////////////////////////////////////////////
			
			// session array�� index Ȯ��
			unsigned short sessionIndex = 0;
			CLockFreeStack<unsigned short>* sessionIndexStack = server->_sessionIndexStack;
			sessionIndexStack->pop(&sessionIndex);

			// session Ȯ��
			stSession* sessionArr = server->_sessionArr;
			stSession* session = &sessionArr[sessionIndex];

			unsigned __int64 sessionID = session->_sessionID;

			// ID�� ���� 6����Ʈ�� ���� �޸𸮿� ���� ���� Ƚ��
			// ���� 2����Ʈ�� ���� �ε���

			// session id ����
			if(sessionID == 0){
				// ���� ���� ���
				int sendBufferSize = server->_sendBufferSize;
				int recvBufferSize = server->_recvBufferSize;

				new (session) stSession(sendBufferSize, recvBufferSize);

				session->_sessionID = (unsigned __int64)session;

			} else {
				// ���� ����
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

			server->log(L"accept.txt", LOG_GROUP::LOG_DEBUG, L"[accept] session: 0x%I64x, sock: %I64d\n", sessionID, sock);

			CRingBuffer* recvBuffer = &session->_recvBuffer;
			CLockFreeQueue<CPacketPtr>* sendQueue = &session->_sendQueue;

			// recv, send ���� �ʱ�ȭ
			recvBuffer->pop(recvBuffer->getUsedSize());
			while(sendQueue->getSize() > 0){
				CPacketPtr packet;
				sendQueue->pop(&packet);
				packet.decRef();
			}
				
			InterlockedExchange8((CHAR*)&session->_beRelease, 0);

			InterlockedIncrement64((LONG64*)&server->_sessionCnt);

			CreateIoCompletionPort((HANDLE)sock, iocp, (ULONG_PTR)session, 0);
			
			server->recvPost(session);

			server->onClientJoin(ip, port, sessionID);
			/////////////////////////////////////////////////////////

		}
		/////////////////////////////////////////////////////////
	}

	return 0;
}

void CLanServer::recvPost(stSession* session){
	
	unsigned __int64 sessionID = session->_sessionID;

	short* ioCnt = &session->_ioCnt;
	InterlockedIncrement16(ioCnt);
	
	/////////////////////////////////////////////////////////
	// �̹� recv�� �ɷ��ִ� ��Ȳ�� �ƴ��� üũ
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
	// ������ release �Ǵ� ��Ȳ�� �ƴ��� üũ	
	/////////////////////////////////////////////////////////
	if(session->_beRelease == true){
		InterlockedDecrement16(ioCnt);
		if(*ioCnt == 0){
			release(sessionID);
		}
		return ;
	}
	/////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////
	// recv buffer ������ wsa buf�� ����
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

			disconnect(sessionID);

			InterlockedDecrement16(ioCnt);
			if(*ioCnt == 0){
				release(sessionID);
			}
			if(recvError != 10054){
				log(L"recv.txt", LOG_GROUP::LOG_DEBUG, L"session: 0x%I64x, sock: %I64d, wsaCnt: %d, wsaBuf[0]: 0x%I64x, wsaBuf[1]: 0x%I64x\n", sessionID, sock, wsaCnt, wsaBuf[0], wsaBuf[1]);
			}
			
			return ;
		}
	}
	/////////////////////////////////////////////////////////
//	sp.profileEnd("RECV POST");
}

void CLanServer::sendPost(stSession* session){
	
	unsigned __int64 sessionID = session->_sessionID;

	short* ioCnt = &session->_ioCnt;
	InterlockedIncrement16(ioCnt);
	
	/////////////////////////////////////////////////////////
	// ������ release �Ǵ� ��Ȳ�� �ƴ��� üũ
	/////////////////////////////////////////////////////////
	if(session->_beRelease == true){
		InterlockedExchange8((char*)&session->_isSent, false);
		InterlockedDecrement16(ioCnt);
		if(*ioCnt == 0){
			release(sessionID);
		}
		return ;
	}
	/////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////
	// ���� �����Ͱ� �ִ��� üũ
	/////////////////////////////////////////////////////////
	CLockFreeQueue<CPacketPtr>* sendQueue = &session->_sendQueue;
	int wsaNum;

	unsigned int usedSize = sendQueue->getSize();
	wsaNum = usedSize;
	wsaNum = min(wsaNum, MAX_PACKET);

	if(wsaNum == 0){
	
		InterlockedExchange8((char*)&session->_isSent, false);
		InterlockedDecrement16(ioCnt);
		if(*ioCnt == 0){
			release(sessionID);
		}
		return ;
	}
	/////////////////////////////////////////////////////////


	
	/////////////////////////////////////////////////////////
	// packet�� wsaBuf�� ����
	/////////////////////////////////////////////////////////
	WSABUF wsaBuf[MAX_PACKET];
	session->_packetCnt = wsaNum;
	int packetNum = wsaNum;

	CPacketPtr packet;
	for(int packetCnt = 0; packetCnt < packetNum; ++packetCnt){
		
		sendQueue->pop(&packet);
		wsaBuf[packetCnt].buf = packet.getBufStart();
		wsaBuf[packetCnt].len = packet.getPacketSize();
		memcpy(&session->_packets[packetCnt], &packet, sizeof(CPacketPtr));

	}

	/////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////////
	// wsa send
	/////////////////////////////////////////////////////////
	int sendResult;
	int sendError;

	SOCKET sock = session->_sock;
	OVERLAPPED* overlapped = &session->_sendOverlapped;

	sendResult = WSASend(sock, wsaBuf, wsaNum, nullptr, 0, overlapped, nullptr);

	if(sendResult == SOCKET_ERROR){
		sendError = WSAGetLastError();
		if(sendError != WSA_IO_PENDING){
			disconnect(sessionID);
			InterlockedDecrement16(ioCnt);
			if(*ioCnt == 0){
				release(sessionID);
			}
			InterlockedExchange8((char*)&session->_isSent, false);
			log(L"send.txt", LOG_GROUP::LOG_SYSTEM, L"session: 0x%I64x, sock: %I64d, wsaNum: %d, wsaBuf[0]: 0x%I64x, wsaBuf[1]: 0x%I64x, error: %d\n", sessionID, sock, wsaNum, wsaBuf[0], wsaBuf[1], sendError);
			
		}
	}	
	/////////////////////////////////////////////////////////
	
}

void CLanServer::start(const wchar_t* configFileName){

	CStringParser settingParser(configFileName);

	// �� ����
	_heap = HeapCreate(0, 0, 0);

	// ���� ���Ͽ��� �ʿ��� ������ ����
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

	// ���� ���� ����
	int socketError;
	{
		_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
		if(_listenSocket == INVALID_SOCKET){
			socketError = WSAGetLastError();
			stStartError error(socketError, L"listen socket init Error");
			throw error;
		}
	}

	// ���� ���Ͽ� linger �ɼ� ����
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

	// ���� ���� send buffer 0
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
	
	// ���� ���� ���ε�
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

	// ������ ����
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

	// session �迭 �ʱ�ȭ
	{
		_sessionArr = new stSession[_sessionNum];
		_sessionIndexStack = new CLockFreeStack<unsigned short>();
		for(int sessionCnt = _sessionNum - 1; sessionCnt >= 0 ; --sessionCnt){
			_sessionIndexStack->push(sessionCnt);
		}
	}


	// iocp �ʱ�ȭ
	int iocpError;
	{
		_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, runningWorkerThreadNum);
		if(_iocp == NULL){
			iocpError = GetLastError();
			stStartError error(iocpError, L"create io completion port Error");
			throw error;
		}
	}

	// worker thread �ʱ�ȭ
	{
		_workerThreadNum = createWorkerThreadNum;
		_workerThread = new HANDLE[createWorkerThreadNum];

		for(int threadCnt = 0; threadCnt < createWorkerThreadNum ; ++threadCnt){
			_workerThread[threadCnt] = (HANDLE)_beginthreadex(nullptr, 0, CLanServer::completionStatusFunc, (void*)this, 0, nullptr);
		}

	}

	// accept thread �ʱ�ȭ
	{
		_acceptThread = (HANDLE)_beginthreadex(nullptr, 0, CLanServer::acceptFunc, (void*)this, 0, nullptr);
	}

}

unsigned __int64 CLanServer::getSessionCount(){
	
	return _sessionCnt;
	
}

void CLanServer::checkCompletePacket(stSession* session, CRingBuffer* recvBuffer){
	
	unsigned __int64 sessionID = session->_sessionID;
	unsigned int usedSize = recvBuffer->getUsedSize();

	while(usedSize > sizeof(stHeader)){
		
		// header üũ
		stHeader header;
		recvBuffer->front(sizeof(stHeader), (char*)&header);

		int payloadSize = header.size;
		int packetSize = payloadSize + sizeof(stHeader);

		// ��Ŷ�� recvBuffer�� �ϼ��Ǿ��ٸ�
		if(usedSize >= packetSize){
			
			recvBuffer->pop(sizeof(stHeader));

			CPacketPtr packet;
			packet << header.size;
			recvBuffer->front(payloadSize, packet.getRearPtr());
			packet.moveRear(payloadSize);

			recvBuffer->pop(payloadSize);
			
			unsigned short sessionIndex = sessionID & _sessionIndexMask;
			stSession* session = &_sessionArr[sessionIndex];

			//packet.moveFront(sizeof(stHeader));
			onRecv(sessionID, packet);

			usedSize -= packetSize;

		} else {
			break;
		}

	}
}

void CLanServer::stop(){

	printf("start stop\n");

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