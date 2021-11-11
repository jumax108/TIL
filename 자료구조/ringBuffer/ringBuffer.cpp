
#include <thread>
#include <Windows.h>
#include "ringBuffer.h"

#define min(a, b) (a>b?b:a)
#define max(a, b) (a>b?a:b)

CRingBuffer::CRingBuffer(unsigned int capacity) {

	InitializeSRWLock(&pushSRW);
	InitializeSRWLock(&popSRW);

	_capacity = capacity;
	_buffer = (char*)malloc((unsigned __int64)capacity + 1);
	ZeroMemory(_buffer, (unsigned __int64)capacity + 1);
	memset(_buffer, 0x7f, (unsigned __int64)capacity + 1);
	_rear = 0;
	_front = 0;

}

CRingBuffer::~CRingBuffer() {

	free(_buffer);
}

bool CRingBuffer::push(unsigned int size, const char* buffer) {

		
	unsigned int freeSize = getFreeSize();
	if (freeSize < size) {
		return false;
	}

	unsigned int rearTemp = _rear;

	do {

		int destBufferIndex = (rearTemp + size);
		if (destBufferIndex >= _capacity + 1) {
			destBufferIndex = _capacity + 1;
		}
		int copySize = destBufferIndex - rearTemp;
		memcpy(&_buffer[rearTemp], buffer, copySize);
		rearTemp = (rearTemp + copySize) % (_capacity + 1);
		buffer += copySize;
		size -= copySize;

	} while (size > 0);

	_rear = rearTemp;

	return true;

}

bool CRingBuffer::pop(unsigned int size) {

	unsigned int usedSize = getUsedSize();
	if (usedSize < size) {
		return false;
	}

	_front = (_front + size) % (_capacity + 1);

	return true;
}

bool CRingBuffer::front(unsigned int size, char* buffer) {

	unsigned int usedSize = getUsedSize();
	if (usedSize < size) {
		return false;
	}
	int copySize = 0;

	int frontTemp = _front;

	do {

		int destBufferIndex = (frontTemp + size);
		if (destBufferIndex >= _capacity + 1) {
			destBufferIndex = _capacity + 1;
		}
		copySize = destBufferIndex - frontTemp;
		memcpy(buffer, &_buffer[frontTemp], copySize);
		buffer += copySize;
		size -= copySize;
		frontTemp = (frontTemp + copySize) % (_capacity + 1);

	} while (size > 0);

	return true;

}

unsigned int CRingBuffer::getFreeSize() {

	return  _capacity - getUsedSize();

}

unsigned int  CRingBuffer::getUsedSize() {

	unsigned int rear = _rear;
	unsigned int front = _front;

	if (rear >= front) {

		return rear - front;

	}
	else {

		return rear + _capacity + 1 - front;

	}

}

char* CRingBuffer::getDirectPush() {
	return &_buffer[_rear];
}

char* CRingBuffer::getDirectFront() {
	return &_buffer[_front];
}

bool CRingBuffer::moveFront(unsigned int size) {

	_front = (_front + size) % (_capacity + 1);
	return true;
}

bool CRingBuffer::moveRear(unsigned int size) {

	_rear = (_rear + size) % (_capacity + 1);
	return true;

}

unsigned int CRingBuffer::getDirectFreeSize() {

	unsigned int front = _front;

	if (front > _rear) {
		return front - _rear - 1;
	}
	return _capacity + 1 - _rear - (front == 0);
}

unsigned int CRingBuffer::getDirectUsedSize() {

	unsigned int rear = _rear;

	if (_front <= rear) {
		return rear - _front;
	}
	return _capacity + 1 - _front;
}
