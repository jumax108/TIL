#pragma once

#include <stdexcept>

class cMessageStack{

#pragma region public enum, struct
public:

	enum class eMESSAGE
	{
		KEY_PRESSED,
		DELETE_OBJECT
	};

	struct stMessage {
		eMESSAGE messageCode;
		int param;

		stMessage(eMESSAGE msgCode, int param) {
			messageCode = msgCode;
			this->param = param;
		}
	};
#pragma endregion

public:

	void push(stMessage* msg);
	void pop();
	inline stMessage* front() {
		return msg[size - 1];
	}

	inline int getSize() {
		return size;
	}

	cMessageStack(
		int cap // reserve
	);

	~cMessageStack();

#pragma region private func, var

private:

	stMessage** msg;

	unsigned int size;
	unsigned int cap;

	void resize(
		int resizeCap // 새로 resize 할 크기를 입력합니다.
	);


#pragma endregion

};