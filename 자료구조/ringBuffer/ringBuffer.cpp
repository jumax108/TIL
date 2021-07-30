#include <stdio.h>
#include <Windows.h>

#include "ringBuffer.h"


CRingBuffer::CRingBuffer(UINT capacity) {
	_capacity = capacity;
	_buffer = (BYTE*)malloc(sizeof(BYTE) * capacity);
	_rear = 0;
	_front = 0;
	_size = 0;
}

CRingBuffer::~CRingBuffer() {
	free(_buffer);
}

bool CRingBuffer::push(UINT size, const BYTE *buffer) {

	if (_size + size > _capacity) {
		return false;
	}

	for (UINT bufCnt = 0; bufCnt < size; ++bufCnt) {

		_buffer[_rear] = buffer[bufCnt];
		
		_rear = (_rear + 1) % _capacity;
		_size += 1;
	}

	return true;

}

bool CRingBuffer::pop(UINT size) {

	if (_size < size) {
		return false;
	}

	_front = (_front + size) % _capacity;
	_size -= size;

	return true;
}

bool CRingBuffer::front(UINT size, BYTE* buffer) {

	if (_size < size) {
		return false;
	}

	for (UINT bufCnt = 0; bufCnt < size; ++bufCnt) {
		buffer[bufCnt] = _buffer[(bufCnt + _rear) % _capacity];
	}

	return true;

}