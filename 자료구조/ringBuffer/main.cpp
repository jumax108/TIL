#include <stdio.h>
#include <Windows.h>
#include <time.h>

#include "ringBuffer.h"

#include "SimpleProfiler.h"

SimpleProfiler sp;

CRingBuffer rb(20);

void unitTest() {

	
	BYTE* a = (BYTE*)"12345678 abcdefgh ";

	srand(1008);

	BYTE out[50] = {0,};

	BYTE* strTemp = a;
	 
	int leftChar = 18;

	for (;;) {

		ZeroMemory(out, 50);

		int bufFree = rb.getFreeSize();

		int maxPushNum = (leftChar > bufFree ? bufFree : leftChar);
		int pushSize = rand() % maxPushNum + 1;
		
		while (pushSize > 0) {

			unsigned int directFreeSize = rb.getDirectFreeSize();
			unsigned int actualPushSize;

			if (pushSize > directFreeSize) {
				memcpy(rb.getDirectPush(), strTemp, directFreeSize);
				actualPushSize = directFreeSize;
			}
			else {
				memcpy(rb.getDirectPush(), strTemp, pushSize);
				actualPushSize = pushSize;
			}
			
			rb.moveRear(actualPushSize);
			strTemp += actualPushSize;
			pushSize -= actualPushSize;
			leftChar -= actualPushSize;
		}
		
		/*
		rb.push(pushSize, strTemp);
		strTemp += pushSize;
		leftChar -= pushSize;
		*/

		if (leftChar == 0) {
			strTemp = a;
			leftChar = 18;
		}

		int bufSize = rb.getUsedSize();

		int popSize = rand() % bufSize + 1;

		unsigned char* outTemp = out;
		while (popSize > 0) {

			unsigned int directUsedSize = rb.getDirectUsedSize();
			unsigned int actualPopSize;

			if (popSize > directUsedSize) {
				memcpy(outTemp, rb.getDirectFront(), directUsedSize);
				actualPopSize = directUsedSize;
			}
			else {
				memcpy(outTemp, rb.getDirectFront(), popSize);
				actualPopSize = popSize;
			}

			rb.moveFront(actualPopSize);
			outTemp += actualPopSize;
			popSize -= actualPopSize;

		}

		/*
		rb.front(popSize, out);
		rb.pop(popSize);
		*/


		printf("%s", out);

		//printf("%s %d %d\n", out,rb._front, rb._rear);

		//system("PAUSE>NUL");
	}
	


}

void speedCheck() {

	const char* str = "123456789";
	int strLen = strlen(str);

	const int loopNum = 100000000;

	for (int loopCnt = 0; loopCnt < loopNum; loopCnt++) {

		sp.profileBegin("push");

		rb.push(strLen, (const BYTE*)str);

		sp.profileEnd("push");

		char outbuf[20];

		sp.profileBegin("front");

		rb.front(strLen, (BYTE*)outbuf);

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