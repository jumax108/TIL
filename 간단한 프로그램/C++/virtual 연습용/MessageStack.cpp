#include "MessageStack.h"

void cMessageStack::pop() {
	if (size == 0) {
		throw std::out_of_range("cMessageStack, pop : size가 0입니다.");
	}
	size -= 1;
}

void cMessageStack::push(stMessage* msg) {
	if (size >= cap) {
		resize(cap * 2);
		cap *= 2;
	}

	this->msg[size] = msg;
	size += 1;
}

void cMessageStack::resize(int resizeCap) {

	stMessage** beforeMsg = msg;

	msg = new stMessage * [resizeCap];
	memset(msg, '\0', sizeof(stMessage*) * resizeCap);

	for (int msgCnt = 0; msgCnt < cap; ++msgCnt) {
		msg[msgCnt] = beforeMsg[msgCnt];
	}

	delete[] beforeMsg;
}

cMessageStack::cMessageStack(int cap) {
	this->cap = cap;
	this->msg = new stMessage * [cap];
	this->size = 0;
}

cMessageStack::~cMessageStack() {
	for (int msgCnt = 0; msgCnt < cap; ++msgCnt) {
		delete msg[msgCnt];
	}
	delete[] msg;
}