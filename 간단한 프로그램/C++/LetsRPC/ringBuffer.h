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

	void getUsedSize(unsigned int* size); 
	void getFreeSize(unsigned int* size);

private:

	unsigned char* _buffer;
	unsigned int _capacity;
	unsigned int _rear;
	unsigned int _front;

};