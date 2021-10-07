#pragma once

class CRingBuffer {

	

public:

	CRingBuffer(unsigned int capacity);
	~CRingBuffer();

	bool push(unsigned int size, const unsigned char* buffer);
	bool pop(unsigned int size);
	bool front(unsigned int size, unsigned char* buffer);

	inline int capacity() {
		return _capacity;
	}

	inline int rear() {
		return _rear;
	}

	inline int front() {
		return _front;
	}

	unsigned char* getDirectPush();
	unsigned char* getDirectFront();

	unsigned int getUsedSize();
	unsigned int getFreeSize();

	unsigned int getDirectFreeSize();
	unsigned int getDirectUsedSize();

	bool moveFront(unsigned int);
	bool moveRear(unsigned int);

private:

	unsigned char* _buffer;
	unsigned int _capacity;
	unsigned int _rear;
	unsigned int _front;

};