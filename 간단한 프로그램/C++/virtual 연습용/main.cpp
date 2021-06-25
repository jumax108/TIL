#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>

#include "MessageStack.h"
#include "BaseObject.h"
#include "MovingStar.h"

#define INFINITE_LOOP for(;;)

char buffer[10][31] = {0,};

BaseObject* obj[10];
int objNum = 0;

cMessageStack msgStack(1);
using stMsg = cMessageStack::stMessage;
using eMsg  = cMessageStack::eMESSAGE;

void input() {
	if (_kbhit() == false) {
		return;
	}

	int input = _getch();

	switch (input) {
	case '1':
	case '2':
	case '3':
		msgStack.push(new stMsg(eMsg::KEY_PRESSED, input));
		break;
	}
}

void update() {

	int msgStackNum = msgStack.getSize();

	for (int msgStackCnt = 0; msgStackCnt < msgStackNum; ++msgStackCnt) {
		stMsg* msg = msgStack.front();

		switch (msg->messageCode) {
		case eMsg::KEY_PRESSED:
			switch (msg->param) {
			case '1':
			case '2':
			case '3':
				for (int objCnt = 0; objCnt < 10; objCnt++) {
					if (obj[objCnt] == nullptr) {
						obj[objCnt] = new MovingStar(msg->param - '0', objCnt);
						break;
					}
				}
				//objNum += 1;
				break;
			}
			break;
		case eMsg::DELETE_OBJECT:
			delete obj[msg->param];
			obj[msg->param] = nullptr;
			break;
		}
		msgStack.pop();
	}

	for (int objCnt = 0; objCnt < 10; ++objCnt) {
		if (obj[objCnt] != nullptr) {
			bool isNotDel = obj[objCnt]->update();
			if (isNotDel == false) {
				msgStack.push(new stMsg(eMsg::DELETE_OBJECT, objCnt));
			}

		}
	}
}

void bufferClear() {
	memset(buffer, '\0', 10 * 31);
}

void render() {

	for (int i = 0; i < 10; i++) {
		printf_s("%s\n", buffer[i]);
	}

	bufferClear();

	for (int objCnt = 0; objCnt < 10; ++objCnt) {
		if (obj[objCnt] != nullptr) {
			obj[objCnt]->render();
		}
	}
}

int main() {

	INFINITE_LOOP
	{
		input();
		update();
		system("cls");
		render();
		Sleep(100);
	}


	return 0;
}