#pragma once

class CRingBuffer {

public:

	CRingBuffer() {}
	CRingBuffer(UINT capacity);
	~CRingBuffer();

	bool push(UINT size, const BYTE* buffer);
	bool pop(UINT size);
	bool front(UINT size, BYTE* buffer);

	inline int size() {
		return _size;
	}

	inline int capacity() {
		return _capacity;
	}

	inline int rear() {
		return _rear;
	}

	inline int front() {
		return _front;
	}

private:

	BYTE* _buffer;
	UINT _rear;
	UINT _front;
	UINT _size;
	UINT _capacity;

};