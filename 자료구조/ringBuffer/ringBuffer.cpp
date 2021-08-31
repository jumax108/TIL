#include <stdio.h>
#include <Windows.h>

#include "ringBuffer.h"

#define min(a, b) (a>b?b:a)
#define max(a, b) (a>b?a:b)

CRingBuffer::CRingBuffer(unsigned int capacity) {
	_capacity = capacity;
	_buffer = (unsigned char*)malloc(capacity + 1);
	ZeroMemory(_buffer, capacity + 1);
	memset(_buffer, 0x7f, capacity + 1);
	_rear = 0;
	_front = 0;

}

CRingBuffer::~CRingBuffer() {
	free(_buffer);
}

bool CRingBuffer::push(unsigned int size, const unsigned char* buffer) {


	unsigned int freeSize;
	getFreeSize(&freeSize);
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

	unsigned int usedSize;
	getUsedSize(&usedSize);
	if (usedSize < size) {
		return false;
	}

	_front = (_front + size) % (_capacity + 1);

	return true;
}

bool CRingBuffer::front(unsigned int size, unsigned char* buffer) {

	unsigned int usedSize;
	getUsedSize(&usedSize);
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

void CRingBuffer::getFreeSize(unsigned int* size) {

	unsigned int usedSize;
	getUsedSize(&usedSize);
	*size = _capacity - usedSize;

}

void CRingBuffer::getUsedSize(unsigned int* size) {

	if (_rear >= _front) {

		*size = _rear - _front;

	}
	else {

		*size = _capacity + 1 - _front;
		*size += _rear;

	}

}