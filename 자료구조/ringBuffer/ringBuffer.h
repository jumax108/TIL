#pragma once

class CRingBuffer {

public:

	CRingBuffer(UINT capacity);
	~CRingBuffer();

	bool push(UINT size, const BYTE* buffer);
	bool pop(UINT size);
	bool front(UINT size, BYTE* buffer);

	inline int capacity() {
		return _capacity;
	}

	inline int rear() {
		return _rear;
	}

	inline int front() {
		return _front;
	}

	void getUsedSize(UINT* size); 
	void getFreeSize(UINT* size);

private:

	BYTE* _buffer;
	UINT _capacity;
	UINT _rear;
	UINT _front;

};