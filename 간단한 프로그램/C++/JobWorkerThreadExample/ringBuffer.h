#pragma once

class CRingBuffer {

public:

	CRingBuffer(unsigned int capacity);
	~CRingBuffer();

	bool push(unsigned int size, const void* buffer);
	bool pop(unsigned int size);
	bool front(unsigned int size, void* buffer);

	inline int capacity() {
		return _capacity;
	}

	inline int rear() {
		return _rear;
	}

	inline int front() {
		return _front;
	}

	char* getDirectPush();
	char* getDirectFront();

	unsigned int getUsedSize();
	unsigned int getFreeSize();

	unsigned int getDirectFreeSize();
	unsigned int getDirectUsedSize();

	void pushLock() {
		EnterCriticalSection(&pushCriticalSection);
	}
	void pushUnlock() {
		LeaveCriticalSection(&pushCriticalSection);
	}

	void popLock() {
		EnterCriticalSection(&popCriticalSection);
	}
	void popUnlock() {
		LeaveCriticalSection(&popCriticalSection);
	}

	bool moveFront(unsigned int);
	bool moveRear(unsigned int);

private:

	CRITICAL_SECTION pushCriticalSection;
	CRITICAL_SECTION popCriticalSection;

	char* _buffer;
	unsigned int _capacity;
	unsigned int _rear;
	unsigned int _front;

};