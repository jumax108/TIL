#pragma once

namespace my {
	template<typename T>
	class stack {

	protected:
		unsigned int capacity;
		T* value;
		alignas(64) unsigned int topIndex;

	public:
		bool push(
			const T* in // push data
		);

		bool pop(
			T* const out // for return
		);

		// get used stack size
		void getSize(
			unsigned int* const out // for return
		);

		// get maximum stack size
		void getCapacity(
			unsigned int* const out // for return
		);

		stack();
		stack(
			unsigned int capacity // minimum capacity is 1
		);

		~stack();
	};

	template<typename T>
	class variableLengthStack: public stack<T> {
	public:

		bool resize(
			unsigned int capacity // change stack size
		);

		// if stack is full, change capacity auto
		bool push(
			const T* in // push data
		);

		variableLengthStack(
			unsigned int capacity
		);

		~variableLengthStack();
	};
}

#include "myStack.cpp"