#ifndef __UNSAFE_OBJECT_FREE_LIST__

#define __UNSAFE_OBJECT_FREE_LIST__

template<typename T>
class CUnsafeObjectFreeList{

public:

	CUnsafeObjectFreeList(HANDLE heap, int _capacity = 0);
	~CUnsafeObjectFreeList();

	T* allocObject(HANDLE thread = nullptr);

	int freeObject(T* data, HANDLE thread = nullptr);

	inline unsigned int getCapacity() { return _capacity; }
	inline unsigned int getUsedCount() { return _usedCnt; }

private:

	struct stNode {

		stNode() {
			_nextNode = nullptr;
		}

		// alloc �Լ����� ������ ���� ������
		T _data;

		// alloc �Լ����� �Ҵ� ������ �ϱ� ����
		stNode* _nextNode;

	};

	struct stLogLine{

		char _code;
		unsigned __int64 _logID;
		HANDLE _thread;

		void* _nowTopNode;
		void* _nextTopNode;
		void* _pushNode;    // ���� ������ ���
		void* _popNode;     // ���� ���� ���

	};

	stLogLine _log[65536];
	unsigned __int64 _logID;
	void* const NO_LOG_DATA = (void*)0xEEEEEEEEEEEEEEEE;

	// �޸� �Ҵ�, ������ ���� ��
	HANDLE _heap;

	// ��� ������ ��带 ����Ʈ�� ���·� �����մϴ�.
	// �Ҵ��ϸ� �����մϴ�.
	stNode* _freeNode;

	// ��ü ��� ����
	unsigned int _capacity;

	// ���� �Ҵ�� ��� ����
	unsigned int _usedCnt;

	// ������ ��ȭ Ƚ�� = push, pop Ƚ��
	unsigned __int64 _stackChangeCnt;

	unsigned __int64 _ptrMask = 0x000007FFFFFFFFFF;

	// �Ҵ� ���� ��� ��ü�� ����Ʈ�Դϴ�.
	// �Ҹ��ڿ��� ���� �� �����մϴ�.
	stNode* _allPtrList;

};
template <typename T>
CUnsafeObjectFreeList<T>::CUnsafeObjectFreeList(HANDLE heap, int size) {

	_allPtrList = nullptr;
	ZeroMemory(&_log, sizeof(_log));
	_stackChangeCnt = 0;

	_freeNode = nullptr;

	_capacity = size;
	_usedCnt = 0;

	_logID = 0;

	_heap = heap;

	if (size == 0) {
		return;
	}

	for(int nodeCnt = 0; nodeCnt < size; ++nodeCnt){
		stNode* node = new stNode;
		node->_nextNode = _freeNode;
		_freeNode = node;
	}

}

template <typename T>
CUnsafeObjectFreeList<T>::~CUnsafeObjectFreeList() {

	while (_allPtrList != nullptr) {
		stNode* nextNode = _allPtrList->_nextNode;
		delete(_allPtrList);
		_allPtrList = nextNode;
	}

}

template<typename T>
T* CUnsafeObjectFreeList<T>::allocObject(HANDLE thread) {

    /*
	*  log code
	*  ���� �ڸ� 
	*      1, ��з� alloc
	*  ���� �ڸ�
	*      1 : alloc ����
	*      3 : InterlockedCompare ����
	*      4 : InterlockedCompareExchange �Ϸ� ��, while Ż��
	*      9 : ���� �ִ� ��尡 ��� new�� ���� �߰� �Ҵ�
    */

	stNode* allocNode = nullptr;
	stNode* nextNode = nullptr;
	stNode* freeNode = nullptr;
	void* nextPtr = nullptr;
	void* freePtr = nullptr;

	{
		// 1 : alloc ����
		unsigned __int64 logID = InterlockedIncrement64((LONG64*)&_logID);
		unsigned short logIndex = (unsigned short)logID;
		stLogLine* logLine = &_log[logIndex];
		logLine->_code				= 0x11;
		logLine->_logID				= logID;
		logLine->_thread			= thread;
		logLine->_nowTopNode		= NO_LOG_DATA;
		logLine->_nextTopNode		= NO_LOG_DATA;
		logLine->_pushNode			= NO_LOG_DATA;
		logLine->_popNode			= NO_LOG_DATA;
	}

	do {
		
		freePtr = _freeNode;
		freeNode = (stNode*)((unsigned __int64)freePtr & _ptrMask);

		if (freeNode == nullptr) {

			allocNode = new stNode;
			InterlockedIncrement(&_capacity);

			{
				// 9 : ���� �ִ� ��尡 ��� new�� ���� �߰� �Ҵ�
				unsigned __int64 logID = InterlockedIncrement64((LONG64*)&_logID);
				unsigned short logIndex = (unsigned short)logID;
				stLogLine* logLine = &_log[logIndex];
				logLine->_code				= 0x19;
				logLine->_logID				= logID;
				logLine->_thread			= thread;
				logLine->_nowTopNode		= freeNode;
				logLine->_nextTopNode		= freeNode;
				logLine->_pushNode			= NO_LOG_DATA;
				logLine->_popNode			= allocNode;
			}

			break;

		} 
	
		allocNode = freeNode;

		nextNode = freeNode->_nextNode;
		nextPtr = (void*)((unsigned __int64)nextNode | (_stackChangeCnt << 43));
		
		{
			// 3 : InterlockedCompare ����
			unsigned __int64 logID = InterlockedIncrement64((LONG64*)&_logID);
			unsigned short logIndex = (unsigned short)logID;
			stLogLine* logLine = &_log[logIndex];
			logLine->_code				= 0x13;
			logLine->_logID				= logID;
			logLine->_thread			= thread;			
			logLine->_nowTopNode		= freeNode;
			logLine->_nextTopNode		= nextNode;
			logLine->_pushNode			= NO_LOG_DATA;
			logLine->_popNode			= allocNode;
		}

	} while ( InterlockedCompareExchange64( (LONG64*)&_freeNode, (LONG64)nextPtr, (LONG64)freePtr ) != (LONG64)freePtr );
	
	{
		// 4 : InterlockedCompareExchange �Ϸ� ��, while Ż��
		unsigned __int64 logID = InterlockedIncrement64((LONG64*)&_logID);
		unsigned short logIndex = (unsigned short)logID;
		stLogLine* logLine = &_log[logIndex];
		logLine->_code				= 0x14;
		logLine->_logID				= logID;
		logLine->_thread			= thread;		
		logLine->_nowTopNode		= freeNode;
		logLine->_nextTopNode		= nextNode;
		logLine->_pushNode			= NO_LOG_DATA;
		logLine->_popNode			= allocNode;
	}


	InterlockedIncrement(&_usedCnt);
	InterlockedIncrement64((LONG64*)&_stackChangeCnt);

	return &(allocNode->_data);
}

template <typename T>
int CUnsafeObjectFreeList<T>::freeObject(T* data, HANDLE thread) {


	 /*
	 *  log code
	 *  ���� �ڸ� 
	 *      2, ��з� free
	 *  ���� �ڸ�
	 *      1 : free ����
	 *      3 : InterlockedCompare ����
	 *      4 : InterlockedCompareExchange �Ϸ� ��, while Ż��
     */

	stNode* usedNode = (stNode*)data;
	stNode* freeNode = nullptr;
	void* usedPtr;
	void* freePtr;
	
	{
		// 1 : free ����
		unsigned __int64 logID = InterlockedIncrement64((LONG64*)&_logID);
		unsigned short logIndex = (unsigned short)logID;
		stLogLine* logLine = &_log[logIndex];
		logLine->_code				= 0x21;
		logLine->_logID				= logID;
		logLine->_thread			= thread;
		logLine->_nowTopNode		= NO_LOG_DATA;
		logLine->_nextTopNode		= usedNode;
		logLine->_pushNode			= usedNode;
		logLine->_popNode			= NO_LOG_DATA;
	}

	do {
	
		freePtr = _freeNode;
		freeNode = (stNode*)((unsigned __int64)freePtr & _ptrMask);

		usedNode->_nextNode = freeNode;

		usedPtr = (void*)((unsigned __int64)usedNode | (_stackChangeCnt << 43));

		{
			// 3 : InterlockedCompare ����
			unsigned __int64 logID = InterlockedIncrement64((LONG64*)&_logID);
			unsigned short logIndex = (unsigned short)logID;
			stLogLine* logLine = &_log[logIndex];
			logLine->_code				= 0x23;
			logLine->_logID				= logID;
			logLine->_thread			= thread;
			logLine->_nowTopNode		= freeNode;
			logLine->_nextTopNode		= usedNode;
			logLine->_pushNode			= usedNode;
			logLine->_popNode			= NO_LOG_DATA;
		}
	
	} while( InterlockedCompareExchange64( (LONG64*)&_freeNode, (LONG64)usedPtr, (LONG64)freePtr ) != (LONG64)freePtr );

	{
		// 4 : InterlockedCompareExchange �Ϸ� ��, while Ż��
		unsigned __int64 logID = InterlockedIncrement64((LONG64*)&_logID);
		unsigned short logIndex = (unsigned short)logID;
		stLogLine* logLine = &_log[logIndex];
		logLine->_code				= 0x24;
		logLine->_logID				= logID;
		logLine->_thread			= thread;
		logLine->_nowTopNode		= freeNode;
		logLine->_nextTopNode		= usedNode;
		logLine->_pushNode			= usedNode;
		logLine->_popNode			= NO_LOG_DATA;
	}

	InterlockedDecrement(&_usedCnt);
	InterlockedIncrement64((LONG64*)&_stackChangeCnt);


	return 0;
}
#endif