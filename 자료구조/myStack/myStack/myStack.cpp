#include "myStack.h"
#include <malloc.h>

using namespace my;

template<typename T>
stack<T>::stack() : stack(1) {}

template<typename T>
stack<T>::stack(unsigned int capacity) {
	if (capacity == 0) {
		capacity = 1;
	}

	topIndex = 0;
	this->capacity = capacity;
	value = (int*)malloc(sizeof(int) * capacity);
}

template<typename T>
stack<T>::~stack() {
	free(value);
}

template<typename T>
bool stack<T>::push(const T* in) {
	if (topIndex == capacity) {
		return false;
	}

	value[topIndex] = *in;
	topIndex += 1;

	return true;
}

template<typename T>
bool stack<T>::pop(T* const out) {
	if (topIndex == 0) {
		return false;
	}

	topIndex -= 1;
	*out = value[topIndex];

	return true;
}

template<typename T>
void stack<T>::getSize(unsigned int* const out) {
	*out = topIndex;
}

template<typename T>
void stack<T>::getCapacity(unsigned int* const out) {
	*out = capacity;
}

template<typename T>
variableLengthStack<T>::variableLengthStack(unsigned int capacity) {
	if (capacity == 0) {
		capacity = 1;
	}

	topIndex = 0;
	this->capacity = capacity;
	value = (int*)malloc(sizeof(int) * capacity);
}

template<typename T>
variableLengthStack<T>::~variableLengthStack() {
	free(value);
}

template<typename T>
bool variableLengthStack<T>::resize(unsigned int capacity) {
	
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

template<typename T>
bool variableLengthStack<T>::push(const T* in) {
	if (topIndex == capacity) {
		resize(capacity * 2);
	}

	value[topIndex] = *in;
	topIndex += 1;

	return true;
}

