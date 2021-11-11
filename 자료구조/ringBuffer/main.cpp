#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <thread>
#include <conio.h>

#include "ringBuffer.h"

#include "SimpleProfiler.h"

SimpleProfiler sp;

constexpr int BUFFER_SIZE = 50;
CRingBuffer rb(BUFFER_SIZE);

const char* orgStr = "12345678 abcdefgh ";

unsigned __stdcall unitTestPushFunc(void*);
unsigned __stdcall unitTestPopFunc(void*);

bool beShutdown;

void unitTest() {

	constexpr unsigned int seed = 0;
	srand(seed);

	beShutdown = false;

	HANDLE pushThread = (HANDLE)_beginthreadex(nullptr, 0, unitTestPushFunc, (void*)&seed, 0, nullptr);
	HANDLE popThread = (HANDLE)_beginthreadex(nullptr, 0, unitTestPopFunc, (void*)&seed, 0, nullptr);

	// 스페이스 입력 대기
	{
		while (_getch() != ' ') {
			Sleep(0);
		}
	}

	// 스레드 종료 유도
	{
		beShutdown = true;
	}

	// 스레드 종료 대기
	{
		HANDLE threads[2];
		threads[0] = pushThread;
		threads[1] = popThread;

		WaitForMultipleObjects(2, threads, true, INFINITE);
	}


	// 스레드 정리
	{
		CloseHandle(pushThread);
		CloseHandle(popThread);
	}

	return;

}

void speedCheck() {

	const char* str = "123456789";
	unsigned int strLen = (unsigned int)strlen(str);

	const int loopNum = 100000000;

	for (int loopCnt = 0; loopCnt < loopNum; loopCnt++) {

		sp.profileBegin("push");

		rb.push(strLen, str);

		sp.profileEnd("push");

		char outbuf[20];

		sp.profileBegin("front");

		rb.front(strLen, outbuf);

		sp.profileEnd("front");

		sp.profileBegin("pop");

		rb.pop(strLen);

		sp.profileEnd("pop");


	}

	sp.printToConsole();
	sp.printToFile();

}

int main() {

	unitTest();

	return 0;
}

unsigned __stdcall unitTestPushFunc(void* arg) {

	unsigned int seed = *(unsigned int*)arg;
	srand(seed);

	const char* strIter = orgStr;
	unsigned int strLen = (unsigned int)strlen(orgStr);
	unsigned int strLeftLen = strLen;


	while (beShutdown == false) {

		unsigned int directFreeSize = rb.getDirectFreeSize();

		if (strLeftLen == 0) {
			strIter = orgStr;
			strLeftLen = strLen;
		}

		unsigned int pushMaxSize = min(directFreeSize, strLeftLen);
		if (pushMaxSize == 0) {
			continue;
		}

		unsigned int pushSize = rand() % pushMaxSize + 1;

		memcpy(rb.getDirectPush(), strIter, pushSize);

		rb.moveRear(pushSize);

		strIter += pushSize;
		strLeftLen -= pushSize;

	}

	return 0;
}

unsigned __stdcall unitTestPopFunc(void* arg) {

	unsigned int seed = *(unsigned int*)arg;
	srand(seed);

	char outBuffer[BUFFER_SIZE];

	while (beShutdown == false) {

 		ZeroMemory(outBuffer, BUFFER_SIZE);

		unsigned int directUsedSize = rb.getDirectUsedSize();
		if (directUsedSize == 0) {
			continue;
		}

		unsigned int maxPopSize = min(directUsedSize, BUFFER_SIZE - 1);
		unsigned int popSize = rand() % maxPopSize + 1;

		memcpy(outBuffer, rb.getDirectFront(), popSize);
		rb.moveFront(popSize);

		printf("%s", outBuffer);
		
	}

	return 0;
}