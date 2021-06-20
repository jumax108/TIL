class queue {

	using valueType = int;
	using ptrType = valueType*;

public:

	queue(int cap) {
		_cap = cap;
		_front = 0;
		_rear = 0;
		_size = 0;
		_data = new valueType[cap];
	}

	inline int cap() {
		return _cap;
	}
	inline int size() {
		return _size;
	}

	inline int front() {
		return _front;
	}

	inline valueType frontValue() {
		return _data[_front];
	}


	inline void peek(int idx, ptrType out) {
		*out = _data[idx];
	}

	inline void push(valueType in) {
		if (_size == _cap) {
			return;
		}
		_data[_rear++] = in;
		++_size;
	}

	inline void pop() {
		if (_size == 0) {
			return;
		}
		++_front;
		--_size;
	}


private:

	int _cap;
	int _size;
	int _front;
	int _rear;
	ptrType _data;

};