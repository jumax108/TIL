#pragma once

#include <stdlib.h>

class BigRandom {

	union _valueRange {
		unsigned long long value;
		unsigned char byte[8];
	};

private:
	_valueRange valueRange;
	int byteNum;
public:
	BigRandom(unsigned long long valueRange);

	void setSeed(unsigned int);

	unsigned long long getRandomValue();
};
