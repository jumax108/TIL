#pragma once

#include "myStack.h"
#include <malloc.h>

template<typename T>
my::stack<T>::stack() : stack(1) {}

template<typename T>
my::stack<T>::stack(unsigned int capacity) {
	if (capacity == 0) {
		capacity = 1;
	}

	topIndex = 0;
	this->capacity = capacity;
	value = (int*)malloc(sizeof(int) * capacity);
}

template<typename T>
my::stack<T>::~stack() {
	free(value);
}

template<typename T>
bool my::stack<T>::push(const T* in) {
	if (topIndex == capacity) {
		return false;
	}

	value[topIndex] = *in;
	topIndex += 1;

	return true;
}

template<typename T>
bool my::stack<T>::pop(T* const out) {
	if (topIndex == 0) {
		return false;
	}

	topIndex -= 1;
	*out = value[topIndex];

	return true;
}

template<typename T>
void my::stack<T>::getSize(unsigned int* const out) {
	*out = topIndex;
}

template<typename T>
void my::stack<T>::getCapacity(unsigned int* const out) {
	*out = capacity;
}

template<typename T>
my::variableLengthStack<T>::variableLengthStack(unsigned int capacity) {
	if (capacity == 0) {
		capacity = 1;
	}

	this->topIndex = 0;
	this->capacity = capacity;
	this->value = (int*)malloc(sizeof(int) * capacity);
}

template<typename T>
my::variableLengthStack<T>::~variableLengthStack() {
	free(this->value);
}

template<typename T>
bool my::variableLengthStack<T>::resize(unsigned int capacity) {
	
	if (capacity == 0) {
		return false;
	}
	this->capacity = capacity;

	int* resizingTemp = nullptr;

	if (this->value != nullptr) {
		resizingTemp = this->value;
	}

	this->value = (int*)malloc(sizeof(int)*capacity);
	if (this->value == nullptr) {
		return false;
	}

	if (resizingTemp != nullptr) {
		for (int valueCnt = 0; valueCnt < this->topIndex; ++valueCnt) {
			this->value[valueCnt] = resizingTemp[valueCnt];
		}
	}

	free(resizingTemp);

	return true;
}

template<typename T>
bool my::variableLengthStack<T>::push(const T* in) {
	if (this->topIndex == this->capacity) {
		resize(this->capacity * 2);
	}

	this->value[this->topIndex] = *in;
	this->topIndex += 1;

	return true;
}

