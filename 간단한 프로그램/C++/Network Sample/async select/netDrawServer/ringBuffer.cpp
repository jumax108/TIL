﻿#include <stdio.h>
#include <Windows.h>

#include "ringBuffer.h"

#define min(a, b) (a>b?b:a)
#define max(a, b) (a>b?a:b)

CRingBuffer::CRingBuffer(UINT capacity) {
	_capacity = capacity;
	_buffer = (BYTE*)malloc(sizeof(BYTE) * (capacity + 1));
	ZeroMemory(_buffer, sizeof(BYTE) * (capacity + 1));
	memset(_buffer, 0x7f, capacity + 1);
	_rear = 0;
	_front = 0;
	
}

CRingBuffer::~CRingBuffer() {
	free(_buffer);
}

bool CRingBuffer::push(UINT size, const BYTE *buffer) {
	

	UINT freeSize;
	getFreeSize(&freeSize);
	if (freeSize < size) {
		return false;
	}

	do{
		
		int destBufferIndex = (_rear + size);
		if (destBufferIndex >= _capacity + 1) {
			destBufferIndex = _capacity + 1;
		}
		int copySize = destBufferIndex - _rear;
		memcpy(&_buffer[_rear], buffer, copySize);
		buffer += copySize;
		_rear = (_rear + copySize) % (_capacity + 1);
		size -= copySize;

	} while (size > 0);

	return true;

}

bool CRingBuffer::pop(UINT size) {

	UINT usedSize;
	getUsedSize(&usedSize);
	if (usedSize < size) {
		return false;
	}

	int copySize = 0;

	do {

		int destBufferIndex = (_front + size);
		if (destBufferIndex >= _capacity + 1) {
			destBufferIndex = _capacity + 1;
		}
		copySize = destBufferIndex - _front;
		_front = (_front + copySize) % (_capacity + 1);
		size -= copySize;

	} while (size > 0);

	return true;
}

bool CRingBuffer::front(UINT size, BYTE* buffer) {

	UINT usedSize;
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
		memcpy(buffer,&_buffer[frontTemp], copySize);
		buffer += copySize;
		size -= copySize;
		frontTemp = (frontTemp + copySize) % (_capacity + 1);

	} while (size > 0);

	return true;

}

void CRingBuffer::getFreeSize(UINT* size) {

	UINT usedSize;
	getUsedSize(&usedSize);
	*size = _capacity - usedSize;

}

void CRingBuffer::getUsedSize(UINT* size) {

	if (_rear >= _front) {

		*size = _rear - _front;

	}
	else {

		*size = _capacity + 1 - _front;
		*size += _rear;

	}

}