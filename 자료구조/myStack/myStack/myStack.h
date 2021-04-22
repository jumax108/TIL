#pragma once

namespace my {
	class stack {

	protected:
		unsigned int capacity;
		int* value;
		alignas(64) unsigned int topIndex;

	public:
		bool push(
			int in // push data
		);

		bool pop(
			int* const out // for return
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

	class variableLengthStack: public stack {
	public:

		bool resize(
			unsigned int capacity // change stack size
		);

		// if stack is full, change capacity auto
		bool push(
			int in // push data
		);

		variableLengthStack(
			unsigned int capacity
		);

		~variableLengthStack();
	};
}