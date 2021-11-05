#pragma once

class CRingBuffer {



public:

	CRingBuffer(unsigned int capacity);
	~CRingBuffer();

	bool push(unsigned int size, const char* buffer);
	bool pop(unsigned int size);
	bool front(unsigned int size, char* buffer);

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

	inline void pushLock() {
		AcquireSRWLockExclusive(&pushSRW);
	}
	inline void pushUnlock() {
		ReleaseSRWLockExclusive(&pushSRW);
	}

	inline void popLock() {
		AcquireSRWLockExclusive(&popSRW);
	}
	inline void popUnlock() {
		ReleaseSRWLockExclusive(&popSRW);
	}

	bool moveFront(unsigned int);
	bool moveRear(unsigned int);

private:

	SRWLOCK pushSRW;
	SRWLOCK popSRW;

	char* _buffer;
	unsigned int _capacity;
	unsigned int _rear;
	unsigned int _front;

};