#pragma once

template <typename T>
class CLockFreeStack{

public:

	CLockFreeStack();
	~CLockFreeStack();

	bool push(T data, HANDLE thread = NULL);
	bool pop(T* data, HANDLE thread = NULL);

	unsigned int getSize();

private:

	struct stLogLine{
		
		unsigned __int64 logID;
		HANDLE threadID;

		char code;

		void* topNode;
		void* nextTopNode;
		void* pushNode; // ���� ������ ��� 
		void* popNode;  // ���� ������� ���
		unsigned __int64 stackPushCnt;  // ���� ��ü���� push�� �ݺ��� Ƚ��
		unsigned __int64 stackPopCnt;   // ���� ��ü����  pop�� �ݺ��� Ƚ��
		T data;

	};

	struct stNode{
		
		stNode(){
			_data = NULL;
			_next = nullptr;
		}

		T _data;
		void* _next;
	};

	void* _top;

	// ���� Ƚ���� ����ũ
	unsigned __int64 _reUseCntMask; 
	// stNode �����Ϳ� ����ũ
	unsigned __int64 _pointerMask;  
	
	// ���� �޸𸮸� �˻��ϱ� ���� ����
	// push�ϸ� ��� ������Ű�� 21�� ��Ʈ�� ptr ���� �־ ����
	unsigned __int64 _nodeChangeCnt;
	
	// �α׿� ����
	// push�ϸ� ��� ����
	// ���� ���̸� ���� ��, �Լ��� ������ ���� ���� �����
	unsigned __int64 _pushCnt;
	// �α׿� ����
	// pop�ϸ� ��� ����
	// ���� ���̸� ���� ��, �Լ��� ������ ���� ���� �����
	unsigned __int64 _popCnt;

	unsigned short _capacity;

	unsigned int _size;

	stLogLine _log[65536] = {0,};
	unsigned __int64 _logID = 0;
				void* NO_LOG_PTR	= (void*)0xEEEEEEEEEEEEEEEE;
	unsigned __int64  NO_LOG_DATA	=		 0xEEEEEEEEEEEEEEEE;

	HANDLE _heap;

	CUnsafeObjectFreeList<stNode>* _nodeFreeList;

};
 
template<typename T>
CLockFreeStack<T>::CLockFreeStack(){

	_size = 0;
	_top = 0;

	_pushCnt = 0;

	_reUseCntMask = 0xFFFFF80000000000;
	_pointerMask  = 0x000007FFFFFFFFFF;

	_heap = HeapCreate(0, 0, 0);
	_nodeFreeList = new CUnsafeObjectFreeList<stNode>(_heap, 0);

}

template<typename T>
CLockFreeStack<T>::~CLockFreeStack(){
	delete _nodeFreeList;
}

template <typename T>
bool CLockFreeStack<T>::push(T data, HANDLE thread){

	/*
	* 
	*	log Code
	*	16^1�� �ڸ�
	*		1 : push
	*	16^0�� �ڸ�
	*		1 : �Լ� ���� 
	*		2 : InterlockedCompare ����
	*		3 : �Լ� ����
	*/
	
	
	unsigned __int64 pushCnt = InterlockedIncrement(&_pushCnt);
	stNode* newNode = _nodeFreeList->allocObject(thread);
	newNode->_data = data;
	
	void* newNodePtr = nullptr;
	void* top = nullptr;
	stNode* topNode = nullptr;
	
	// 1 : �Լ� ����
	{
		unsigned __int64 logID = InterlockedIncrement64((LONG64*)&_logID);
		unsigned short logIndex = (unsigned short)logID;
		stLogLine* logLine = &_log[logIndex];
		logLine->code			= 0x11;
		logLine->logID			= logID;
		logLine->threadID		= thread;
		logLine->topNode		= NO_LOG_PTR;
		logLine->nextTopNode	= NO_LOG_PTR;
		logLine->pushNode		= NO_LOG_PTR;
		logLine->popNode		= NO_LOG_PTR;
		logLine->stackPushCnt	= pushCnt;
		logLine->stackPopCnt	= NO_LOG_DATA;
		logLine->data			= data;
	}


	do {
		
		top = _top;

		topNode = (stNode*)((unsigned __int64)top & _pointerMask);
		newNode->_next = top;
		newNodePtr = (void*)((_nodeChangeCnt << 43) | (unsigned __int64)newNode);
		
		// 2 : InterlockedCompare ����
		{
			unsigned __int64 logID = InterlockedIncrement64((LONG64*)&_logID);
			unsigned short logIndex = (unsigned short)logID;
			stLogLine* logLine = &_log[logIndex];
			logLine->code			= 0x12;
			logLine->logID			= logID;
			logLine->threadID		= thread;
			logLine->topNode		= topNode;
			logLine->nextTopNode	= newNode;
			logLine->pushNode		= newNode;
			logLine->popNode		= NO_LOG_PTR;
			logLine->stackPushCnt	= pushCnt;
			logLine->stackPopCnt	= NO_LOG_DATA;
			logLine->data			= data;
		}
		
	} while(InterlockedCompareExchange64((__int64*)&_top, (__int64)newNodePtr, (__int64)top) != (__int64)top);
	
	InterlockedIncrement64((LONG64*)&_nodeChangeCnt);
	// 3 : �Լ� ����
	{
		unsigned __int64 logID = InterlockedIncrement64((LONG64*)&_logID);
		unsigned short logIndex = (unsigned short)logID;
		stLogLine* logLine = &_log[logIndex];
		logLine->code			= 0x13;
		logLine->logID			= logID;
		logLine->threadID		= thread;
		logLine->topNode		= topNode;
		logLine->nextTopNode	= newNode;
		logLine->pushNode		= newNode;
		logLine->popNode		= NO_LOG_PTR;
		logLine->stackPushCnt	= pushCnt;
		logLine->stackPopCnt	= NO_LOG_DATA;
		logLine->data			= data;
	}


	return true;

}

template<typename T>
bool CLockFreeStack<T>::pop(T* data, HANDLE thread){

	/*
	* 
	*	log Code
	*	16^1�� �ڸ�
	*		2 : pop
	*	16^0�� �ڸ�
	*		1 : �Լ� ����
	*		2 : InterlockedCompare ����
	*		3 : InterlockedCompareExchange �Ϸ�
	*/
	
	unsigned __int64 popCnt = InterlockedIncrement64((LONG64*)&_popCnt);

	// 1 : �Լ� ����
	{
		unsigned __int64 logID = InterlockedIncrement64((LONG64*)&_logID);
		unsigned short logIndex = (unsigned short)logID;
		stLogLine* logLine = &_log[logIndex];
		logLine->code			= 0x21;
		logLine->logID			= logID;
		logLine->threadID		= thread;
		logLine->topNode		= NO_LOG_PTR;
		logLine->nextTopNode	= NO_LOG_PTR;
		logLine->pushNode		= NO_LOG_PTR;
		logLine->popNode		= NO_LOG_PTR;
		logLine->stackPushCnt	= NO_LOG_DATA;
		logLine->stackPopCnt	= popCnt;
		logLine->data			= NULL;
	}

	stNode* topNode			= nullptr;
	stNode* topNextNode		= nullptr;
	void* topNextPtr		= nullptr;
	void* top				= nullptr;


	do {

		top = _top;
		popCnt = _popCnt;

		topNode = (stNode*)((unsigned __int64)top & _pointerMask);
		topNextPtr = topNode->_next;
		topNextNode = (stNode*)((unsigned __int64)topNextPtr & _pointerMask);
		
		// 2 : InterlockedCompare ����
		{
			unsigned __int64 logID = InterlockedIncrement64((LONG64*)&_logID);
			unsigned short logIndex = (unsigned short)logID;
			stLogLine* logLine = &_log[logIndex];
			logLine->code			= 0x22;
			logLine->logID			= logID;
			logLine->threadID		= thread;
			logLine->topNode		= topNode;
			logLine->nextTopNode	= topNextNode;
			logLine->pushNode		= NO_LOG_PTR;
			logLine->popNode		= topNode;
			logLine->stackPushCnt	= NO_LOG_DATA;
			logLine->stackPopCnt	= popCnt;
			logLine->data			= topNode->_data;
		}
		

	} while(InterlockedCompareExchange64((__int64*)&_top, (__int64)topNextPtr, (__int64)top) != (__int64)top);
	
	InterlockedIncrement64((LONG64*)&_nodeChangeCnt);

	// 3 : InterlockedCompareExchange �Ϸ�
	{
		unsigned __int64 logID = InterlockedIncrement64((LONG64*)&_logID);
		unsigned short logIndex = (unsigned short)logID;
		stLogLine* logLine = &_log[logIndex];
		logLine->code			= 0x23;
		logLine->logID			= logID;
		logLine->threadID		= thread;
		logLine->topNode		= topNode;
		logLine->nextTopNode	= topNextNode;
		logLine->pushNode		= NO_LOG_PTR;
		logLine->popNode		= topNode;
		logLine->stackPushCnt	= NO_LOG_DATA;
		logLine->stackPopCnt	= popCnt;
		logLine->data			= topNode->_data;
	}
	

	*data = topNode->_data;

	_nodeFreeList->freeObject(topNode, thread);

	return true;
}

template <typename T>
unsigned int CLockFreeStack<T>::getSize(){
	return _size;
}