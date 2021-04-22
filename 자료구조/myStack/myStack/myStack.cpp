#include "myStack.h"
#include <malloc.h>

using namespace my;

stack::stack() : stack(1) {}
stack::stack(unsigned int capacity) {
	if (capacity == 0) {
		capacity = 1;
	}

	topIndex = 0;
	this->capacity = capacity;
	value = (int*)malloc(sizeof(int) * capacity);
}

stack::~stack() {
	free(value);
}

bool stack::push(int in) {
	if (topIndex == capacity) {
		return false;
	}

	value[topIndex] = in;
	topIndex += 1;

	return true;
}

bool stack::pop(int* const out) {
	if (topIndex == 0) {
		return false;
	}

	topIndex -= 1;
	*out = value[topIndex];

	return true;
}

void stack::getSize(unsigned int* const out) {
	*out = topIndex;
}

void stack::getCapacity(unsigned int* const out) {
	*out = capacity;
}


variableLengthStack::variableLengthStack(unsigned int capacity) {
	if (capacity == 0) {
		capacity = 1;
	}

	topIndex = 0;
	this->capacity = capacity;
	value = (int*)malloc(sizeof(int) * capacity);
}
variableLengthStack::~variableLengthStack() {
	free(value);
}

bool variableLengthStack::resize(unsigned int capacity) {

	if (capacity == 0) {
		return false;
	}
	this->capacity = capacity;

	int* resizingTemp = nullptr;

	if (value != nullptr) {
		resizingTemp = value;
	}

	value = (int*)malloc(sizeof(int)*capacity);
	if (value == nullptr) {
		return false;
	}

	if (resizingTemp != nullptr) {
		for (int valueCnt = 0; valueCnt < topIndex; ++valueCnt) {
			value[valueCnt] = resizingTemp[valueCnt];
		}
	}

	free(resizingTemp);

	return true;
}

bool variableLengthStack::push(int in) {
	if (topIndex == capacity) {
		resize(capacity * 2);
	}

	value[topIndex] = in;
	topIndex += 1;

	return true;
}

