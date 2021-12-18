#pragma once

extern CDump dump;

template <typename T>
class CLockFreeQueue{

public:

	CLockFreeQueue();

	bool push(T data, HANDLE threadHandle = NULL);
	bool pop(T* data, HANDLE threadHandle = NULL);

	unsigned __int64 getSize();

private:

	struct stNode{
		stNode(){
			_next = nullptr;
			_data = NULL;
		}
		void* _next;
		T _data;
	};
	
	struct stLogLine{

		char _code;
		HANDLE _thread;
		unsigned __int64 _logCnt;

		// ���°�� ������ �Լ������� ��Ÿ���ϴ�.
		unsigned __int64 _functionCnt;
		
		void*	_headPtr;
		stNode* _headNode;
		stNode* _nextHeadNode;
		stNode* _popNode;
		void*	_popData;

		void*	_tailPtr;
		stNode* _tailNode;
		stNode* _tailNodeNext;
		stNode* _nextTailNode;
		stNode* _pushNode;
		void*   _pushPtr;
		void*	_pushData;
	};
	
	void* _head;
	void* _tail;

	unsigned __int64 _size = 0;

	// ���°�� �Լ� ȣ������ Ȯ��, push pop ���� �ÿ� �����ϰ� �����մϴ�.
	// ���� ���̸� ������ ������, ������ �Լ��� ������ �ʾ����� �����մϴ�.
	unsigned __int64 _functionCnt = 0;
	unsigned __int64 _useCnt = 0;

	unsigned __int64 _useCntMask	= 0xFFFFF80000000000;
	unsigned __int64 _pointerMask	= 0x000007FFFFFFFFFF;

	stLogLine _log[65536];
	unsigned __int64 _logCnt = 0;
	stNode* const NO_LOG_PTR = (stNode*)0xEEEEEEEEEEEEEEEE;
	const unsigned __int64 NO_LOG_DATA = 0xEEEEEEEEEEEEEEEE;


	HANDLE _heap;
	CUnsafeObjectFreeList<stNode>* _nodeFreeList;

};

template <typename T>
CLockFreeQueue<T>::CLockFreeQueue(){

	_nodeFreeList = new CUnsafeObjectFreeList<stNode>(_heap);

	stNode* node = _nodeFreeList->allocObject();
	_head = node;
	_tail = node;

	_heap = HeapCreate(0, 0, 0);

	ZeroMemory(_log, sizeof(_log));

}

template <typename T>
bool CLockFreeQueue<T>::push(T data, HANDLE threadHandle){
	
	/*
	*	log Code
	*	16^1�� �ڸ�
	*		1 : push
	*	16^0�� �ڸ�
	*		1 : �Լ� ���� 
	*		2 : tail->next�� new node�� �����ϱ� ��
	*       3 : tail�� new node�� �����ϱ� ��
	*		4 : �Լ� ����
	*/

	unsigned __int64 functionCnt = InterlockedIncrement64((LONG64*)&_functionCnt);
	
	stNode* newNode = _nodeFreeList->allocObject();

	// 1 : �Լ� ����
	{
		unsigned __int64 logCnt = InterlockedIncrement64((LONG64*)&_logCnt) - 1;
		unsigned short logIndex = (unsigned short)logCnt;

		stLogLine* logLine = &_log[logIndex];

		logLine->_code			= 0x11;
		logLine->_thread		= threadHandle;
		logLine->_functionCnt	= functionCnt;
		logLine->_logCnt		= logCnt;

		logLine->_headPtr		= _head;
		logLine->_headNode		= NO_LOG_PTR;
		logLine->_nextHeadNode	= NO_LOG_PTR;
		logLine->_popNode		= NO_LOG_PTR;
		logLine->_popData		= NO_LOG_PTR;
		
		logLine->_tailPtr		= _tail;
		logLine->_tailNode		= NO_LOG_PTR;
		logLine->_tailNodeNext	= NO_LOG_PTR;
		logLine->_nextTailNode	= newNode;
		logLine->_pushNode		= newNode;
		logLine->_pushPtr		= (void*)NO_LOG_PTR;
		logLine->_pushData		= (void*)data;

	}

	newNode->_data = data;
	newNode->_next = nullptr;

	void* newPtr;
	void* tail;
	void* tailNextPtr = nullptr;
	void* head;

	stNode* tailNode;
	stNode* headNode;
	stNode* tailNextNode;
	
	unsigned __int64 loopCnt = 0;
	
	unsigned __int64 useCnt;

	do{
		
		if(loopCnt > 0){
			
			// tail�� null�� �ƴ� ������ ����
			while(tailNextPtr != nullptr){

				tail = _tail;
				tailNode = (stNode*)((unsigned __int64)tail & _pointerMask);
				tailNextPtr = tailNode->_next;

				if(tailNextPtr != nullptr){
					InterlockedCompareExchange64((LONG64*)&_tail, (LONG64)tailNextPtr, (LONG64)tail);
				}

			}

		}

		tail = _tail;
		head = _head;
		useCnt = _useCnt;

		tailNode = (stNode*)((unsigned __int64)tail & _pointerMask);
		headNode = (stNode*)((unsigned __int64)head & _pointerMask);
		tailNextPtr = tailNode->_next;
		tailNextNode = (stNode*)((unsigned __int64)tailNextPtr & _pointerMask);
		
		newPtr = (void*)((useCnt << 43) | (unsigned __int64)newNode);

		loopCnt+=1;
			
	} while( InterlockedCompareExchange64((LONG64*)&tailNode->_next, (LONG64)newPtr, (LONG64)nullptr ) != (LONG64)nullptr );
	_useCnt = useCnt + 1;
	//InterlockedIncrement64((LONG64*)&_useCnt);
				
	tailNextPtr = tailNode->_next;
	tailNextNode = (stNode*)((unsigned __int64)tailNextPtr & _pointerMask);

	bool result = InterlockedCompareExchange64((LONG64*)&_tail, (LONG64)newPtr, (LONG64)tail) != (LONG64)tail;
	
	InterlockedIncrement64((LONGLONG*)&_size);

	// 4 : �Լ� ����
	{
		unsigned __int64 logCnt = InterlockedIncrement64((LONG64*)&_logCnt) - 1;
		unsigned short logIndex = (unsigned short)logCnt;

		stLogLine* logLine = &_log[logIndex];

		logLine->_code			= 0x14;
		logLine->_thread		= threadHandle;
		logLine->_functionCnt	= functionCnt;
		logLine->_logCnt		= logCnt;

		logLine->_headPtr		= head;
		logLine->_headNode		= NO_LOG_PTR;
		logLine->_nextHeadNode	= NO_LOG_PTR;
		logLine->_popNode		= NO_LOG_PTR;
		logLine->_popData		= NO_LOG_PTR;
		
		logLine->_tailPtr		= tail;
		logLine->_tailNode		= tailNode;
		logLine->_tailNodeNext	= tailNextNode;
		logLine->_nextTailNode	= newNode;
		logLine->_pushNode		= newNode;
		logLine->_pushPtr		= newPtr;
		logLine->_pushData		= (void*)data;

	}

	
	return true;

}

template <typename T>
bool CLockFreeQueue<T>::pop(T* data, HANDLE threadHandle){
	
	/*
	*	log Code
	*	16^1�� �ڸ�
	*		2 : pop
	*	16^0�� �ڸ�
	*		1 : �Լ� ���� 
	*		2 : InterlockedCompare ����
	*		3 : �Լ� ����
	*/

	unsigned __int64 functionCnt = InterlockedIncrement64((LONG64*)&_functionCnt);

	// 1 : �Լ� ����
	{
		unsigned __int64 logCnt = InterlockedIncrement64((LONG64*)&_logCnt) - 1;
		unsigned short logIndex = (unsigned short)logCnt;

		stLogLine* logLine = &_log[logIndex];

		logLine->_code			= 0x21;
		logLine->_thread		= threadHandle;
		logLine->_functionCnt	= functionCnt;
		logLine->_logCnt		= logCnt;

		logLine->_headPtr		= _head;
		logLine->_headNode		= NO_LOG_PTR;
		logLine->_nextHeadNode	= NO_LOG_PTR;
		logLine->_popNode		= NO_LOG_PTR;
		logLine->_popData		= NO_LOG_PTR;
		
		logLine->_tailPtr		= _tail;
		logLine->_tailNode		= NO_LOG_PTR;
		logLine->_tailNodeNext	= NO_LOG_PTR;
		logLine->_nextTailNode	= NO_LOG_PTR;
		logLine->_pushNode		= NO_LOG_PTR;
		logLine->_pushPtr		= (void*)NO_LOG_PTR;
		logLine->_pushData		= NO_LOG_PTR;

	}

	void* popPtr;
	void* tail;
	void* tailNextPtr = nullptr;
	void* head;
	void* nextHeadPtr;

	stNode* popNode;
	stNode* headNode;
	stNode* eraseNode;
	stNode* tailNode;
	stNode* tailNextNode = nullptr;

	T popNodeData = NULL;

	unsigned __int64 loopCnt = 0;
	
	do{
		do { 
			head = _head;
			tail = _tail;

			headNode = (stNode*)((unsigned __int64)head & _pointerMask);
			popPtr = headNode->_next;
			popNode = (stNode*)((unsigned __int64)popPtr & _pointerMask);
			tailNode = (stNode*)((unsigned __int64)tail & _pointerMask);
		
			if(tailNode != nullptr){
				tailNextPtr = tailNode->_next;
				tailNextNode = (stNode*)((unsigned __int64)tailNextPtr & _pointerMask);
			}

			unsigned __int64 useCnt = _useCnt;

			eraseNode = headNode;

			nextHeadPtr = (void*)((unsigned __int64)popNode | (useCnt << 43));

		} while(popNode == nullptr);

		popNodeData = popNode->_data;

		loopCnt += 1;

	}while(  InterlockedCompareExchange64((LONG64*)&_head, (LONG64)nextHeadPtr, (LONG64)head) != (LONG64)head);
	
	*data = popNodeData;
	
	_nodeFreeList->freeObject(eraseNode);

	// 3 : �Լ� ����
	{		
		unsigned __int64 logCnt = InterlockedIncrement64((LONG64*)&_logCnt) - 1;
		unsigned short logIndex = (unsigned short)logCnt;

		stLogLine* logLine = &_log[logIndex];

		logLine->_code			= 0x23;
		logLine->_thread		= threadHandle;
		logLine->_functionCnt	= functionCnt;
		logLine->_logCnt		= logCnt;

		logLine->_headPtr		= head;
		logLine->_headNode		= headNode;
		logLine->_nextHeadNode	= popNode;
		logLine->_popNode		= popNode;
		logLine->_popData		= (void*)popNodeData;
		
		logLine->_tailPtr		= tail;
		logLine->_tailNode		= tailNode;
		logLine->_tailNodeNext	= tailNextNode;
		logLine->_nextTailNode	= NO_LOG_PTR;
		logLine->_pushNode		= NO_LOG_PTR;
		logLine->_pushPtr		= (void*)NO_LOG_PTR;

	}

	return true;
}

template <typename T>
unsigned __int64 CLockFreeQueue<T>::getSize(){
	return _size;
}