#pragma once

#include <malloc.h>
#include <stdexcept>

namespace my {
	template<typename T>
	class stack {


	public:
		bool push(
			const T in // ������ �Է�
		);

		void pop();

		int front() const;

		unsigned int getSize() const; // ���� ��뷮�� ����ϴ�.

		unsigned int getCapacity() const; // ���� �Ҵ緮�� ����ϴ�.

		stack();
		stack(
			unsigned int capacity = 1 // �ּ� �Ҵ緮 1
		);

		~stack();
	private:
		unsigned int capacity;
		T* value;
		unsigned int topIndex;

	private:
		void resize();
	};
}

template<typename T>
my::stack<T>::stack() : stack(1) {}

template<typename T>
my::stack<T>::stack(unsigned int capacity) {
	topIndex = 0;
	this->capacity = capacity;
	value = new T[capacity];
}

template<typename T>
my::stack<T>::~stack() {
	delete[] value;
}

template<typename T>
bool my::stack<T>::push(const T in) {
	if (topIndex == capacity) {
		return false;
	}

	value[topIndex] = in;
	topIndex += 1;

	return true;
}

template<typename T>
void my::stack<T>::pop() {
	if (topIndex == 0) {
		throw std::out_of_range("Stack::pop, stack�� �ƹ� �͵� �����ϴ�.");
	}

	topIndex -= 1;
}

template<typename T>
int my::stack<T>::front() const {
	return value[topIndex - 1];
}

template<typename T>
unsigned int my::stack<T>::getSize() const {
	return topIndex;
}

template<typename T>
unsigned int my::stack<T>::getCapacity() const {
	return capacity;
}