#pragma once

#include <WinSock2.h>
#pragma comment(lib,"ws2_32")
#include <WS2tcpip.h>
#include <stdexcept>
#include <thread>
#include <new>
#include <windows.h>
#include <crtdbg.h>
#include <DbgHelp.h>


#include <stack>


#include "../common/dump.h"
#include "../common/log.h"
#include "../common/SimpleProfiler.h"
#include "../common/ObjectFreeListTLS.h"
#include "../common/stack.h"
#include "../common/stringParser.h"
#include "../common/ringBuffer.h"
#include "../common/protocolBuffer.h"
#include "../common/lockFreeQueue.h"
#include "../common/lockFreeStack.h"
#include "common.h"
#include "packetPtr_NetServer.h"

constexpr int MTU = 1500;
constexpr int PROTOCOL_HEADER_SIZE = 44;
constexpr int MSS = MTU - PROTOCOL_HEADER_SIZE;

extern SimpleProfiler sp;

extern CDump dump;

#define MAX_PACKET 500

class CChatServer;

class CNetServer{

	friend class CChatServer;

public:

	CNetServer();

	// �ʿ��� �޸𸮸� �Ҵ��ϰ� ������ �����մϴ�.
	void start(const wchar_t* configFileName); 
	// ��� �޸𸮸� �����ϰ� ������ �����մϴ�.
	void stop(); 
	
	// ���� �������� ���� ���� ��ȯ�մϴ�.
	unsigned __int64 getSessionCount();
	
	// sessionID �� �ش��ϴ� ���� �����մϴ�.
	bool disconnect(unsigned __int64 sessionID); 
	// sessinoID �� �ش��ϴ� ���ǿ� ������ �����մϴ�.
	bool sendPacketPostIOCP(unsigned __int64 sessionID, CPacketPtrNet packet);
	bool sendPacketPostSend(unsigned __int64 sessionID, CPacketPtrNet packet);
	 
	// Ŭ���̾�Ʈ�� ������ �õ��� ���¿��� ȣ��˴ϴ�.
	// ��ȯ�� ���� ���� ������ ����մϴ�.
	// return true = ���� ��, ���� �ʱ�ȭ
	// return false = ������ ����
	virtual bool onConnectRequest(unsigned int ip, unsigned short port) = 0;
	// Ŭ���̾�Ʈ�� ������ �Ϸ��� ���¿��� ȣ��˴ϴ�.
	virtual void onClientJoin(unsigned int ip, unsigned short port, unsigned __int64 sessionID) = 0;
	// Ŭ���̾�Ʈ�� ������ �����Ǹ� ȣ��˴ϴ�.
	virtual void onClientLeave(unsigned __int64 sessionID) = 0;

	// Ŭ���̾�Ʈ���� �����͸� �����ϸ� ȣ��˴ϴ�.
	virtual void onRecv(unsigned __int64 sessionID, CPacketPtr pakcet) = 0;
	// Ŭ���̾�Ʈ���Լ� �����͸� ���޹����� ȣ��˴ϴ�.
	virtual void onSend(unsigned __int64 sessionID, int sendSize) = 0;

	// ���� ��Ȳ���� ȣ��˴ϴ�.
	virtual void onError(int errorCode, const wchar_t* errorMsg) = 0;

	// ���� ���� �� ������ �߻��ϸ� throw �˴ϴ�.
	struct stStartError{
		int _errorCode;
		const wchar_t* _errorMessage;
		stStartError(int errorCode, const wchar_t* errorMessage){
			_errorCode = errorCode;
			_errorMessage = errorMessage;
		}
	};

	unsigned __int64 getRecvTPS();
	unsigned __int64 getSendTPS();
	unsigned __int64 getAcceptTPS();
	unsigned __int64 getAcceptTotal();
	
	// max packet num
	int maxPacketNum = 0;
protected:

	struct stSession{
		
		stSession(unsigned int sendBufferSize = 1, unsigned int recvBufferSize = 1):
			_recvBuffer(5000)
		{
			_sessionID = 0;
			_sock = NULL;
			_ip = 0;
			_port = 0;
			_isSent = false;
			_ioCnt = 0;
			_disconnect = false;
			ZeroMemory(&_sendOverlapped, sizeof(OVERLAPPED));
			ZeroMemory(&_recvOverlapped, sizeof(OVERLAPPED));
			_packets = (CPacketPtr*)malloc(sizeof(CPacketPtr) * MAX_PACKET);
			ZeroMemory(_packets, sizeof(CPacketPtr) * MAX_PACKET);
			_packetCnt = 0;

			_recvPosted = false;
		}

		~stSession(){
		}

		// ID�� ���� 6����Ʈ�� ���� �޸𸮿� ���� ���� Ƚ��
		// ���� 2����Ʈ�� ���� �ε���
		unsigned __int64 _sessionID; // ���� ���� �߿��� ������ ���� ID
	
		SOCKET _sock;
	
		unsigned int _ip;
		unsigned short _port;

		CLockFreeQueue<CPacketPtr> _sendQueue;
		CRingBuffer _recvBuffer;
		
		// send�� 1ȸ�� �����ϱ� ���� �÷���
		bool _isSent;
		
		// Disconnect�� ȣ���, IO ��� ���� IO ���� �۾� ���ø� ����
		bool _disconnect;

		// �� 32��Ʈ�� ������ �÷��� ��ȭ�� ioCnt�� 0���� ���ÿ� üũ�ϱ� ����
		alignas(32) bool _release;
		// 32��Ʈ ä��� �� ������ ������� ����
		private: char _temp = 0;
		// recv, send io�� ��û�Ǿ� �ִ� Ƚ���Դϴ�.
		// recv�� �׻� ��û�Ǿ��ֱ� ������ ioCnt�� �ּ� 1�Դϴ�.
		// ioCnt�� 0�̵Ǹ� ������ ���� �����Դϴ�.
		public: short _ioCnt; 

		// recv �Լ� �ߺ� ȣ�� ������
		bool _recvPosted;
		
		OVERLAPPED _sendOverlapped;
		OVERLAPPED _recvOverlapped;

		CPacketPtr* _packets;
		int _packetCnt;
	};

	stSession* _sessionArr;
	unsigned int _sessionNum;
	unsigned __int64 _sessionIndexMask;
	unsigned __int64 _sessionAllocCntMask;
public:
	int _sendBufferSize;
	int _recvBufferSize;

protected:
	HANDLE _acceptThread;
	int _workerThreadNum;
	HANDLE* _workerThread;
	SOCKET _listenSocket;

	alignas(64) CLockFreeStack<unsigned short>* _sessionIndexStack;
	unsigned int _sessionCnt;

	HANDLE _iocp;

	// free list���� �Ҵ��� �� ���
	HANDLE _heap;

	// logger
	CLog log;

	// thread ������ event
	HANDLE _stopEvent;

	// ��ü accept ��
	alignas(64) unsigned int _acceptTotal;	
	unsigned int _acceptCnt;
	unsigned int _recvCnt;
	unsigned int _sendCnt;
	unsigned __int64 _sendBytes = 0;
	unsigned __int64 _recvBytes = 0;
	// recv TPS ������ ����
	alignas(64) unsigned int _recvTPS;
	// send TPS ������ ����
	unsigned int _sendTPS;
	// accept TPS ������ ����
	unsigned int _acceptTPS;

	unsigned int _maxSendQueCap = 0;
	unsigned int _maxRecvBufferCap = 0;

	// tps ó���� ������
	HANDLE _tpsThread;

	short _iocpQueueSize = 0;

	static unsigned __stdcall tpsFunc(void* args);

	void sendPost(stSession* session);
	void recvPost(stSession* session);

	static unsigned __stdcall completionStatusFunc(void* args);
	static unsigned __stdcall acceptFunc(void* args);

	void checkCompletePacket(stSession* session, CRingBuffer* recvBuffer);

	void release(unsigned __int64 sessionID);
};
