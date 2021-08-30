#pragma once

class CRingBuffer {

public:

	CRingBuffer() {}
	CRingBuffer(UINT capacity);
	~CRingBuffer();

	bool push(UINT size, const char* buffer);
	bool pop(UINT size);
	bool front(UINT size, char* buffer);

	inline int capacity() {
		return _capacity;
	}

	inline int rear() {
		return _rear;
	}

	inline int front() {
		return _front;
	}

	inline int getDirectFreeSize() {

		return _capacity + (_front != 0) - _rear;

	}

	inline int getDirectUsedSize() {

		return _capacity +1 - _front;

	}

	inline char* getRearPtr() {
		return &_buffer[_rear];
	}

	inline char* getFrontPtr() {
		return &_buffer[_front];
	}

	inline void moveRear(UINT size) {
		_rear = (_rear + size) % (_capacity + 1);
	}
	inline void moveFront(UINT size) {
		_front = (_front + size) % (_capacity + 1);
	}

	void getUsedSize(UINT* size); 
	void getFreeSize(UINT* size);

private:

	char* _buffer;
	UINT _capacity;
	UINT _rear;
	UINT _front;

};