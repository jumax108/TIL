#include "BigRandom.h"

BigRandom::BigRandom(unsigned long long valueRange) {
	this->valueRange.value = valueRange;
	byteNum = 8;
	for (int byteCnt = 0; byteCnt < 8; byteCnt++) {
		if (this->valueRange.byte[byteCnt] == '\0') {
			byteNum = byteCnt;
			break;
		}
	}
}

void BigRandom::setSeed(unsigned int seed) {
	srand(seed);
}

unsigned long long BigRandom::getRandomValue() {
	unsigned long long result = 0;

	for (int randCnt = 0; randCnt < byteNum; randCnt++) {
		unsigned char byteRndVal = 0;
		byteRndVal = rand() & 0xff;
		result = (result << 8) | byteRndVal;
	}

	return result % valueRange.value;
}