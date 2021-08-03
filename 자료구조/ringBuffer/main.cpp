#include <stdio.h>
#include <Windows.h>
#include "ringBuffer.h"

int main() {

	constexpr int BUFFER_SIZE = 10;

	CRingBuffer ringBuffer(BUFFER_SIZE);

	const BYTE* str = (const BYTE*)"HelloWorld";
	const BYTE* str2 = (const BYTE*)"World";

	int strLen = strlen((const char*)str);
	int str2Len = strlen((const char*)str2);


	BYTE* res = (BYTE*)malloc(sizeof(BYTE) * (BUFFER_SIZE + 1));
	ZeroMemory(res, BUFFER_SIZE + 1);

	ringBuffer.push(str2Len, str2);
	ringBuffer.front(str2Len, res);
	ringBuffer.pop(str2Len);
	printf("%s\n", res);

	ringBuffer.push(strLen, str);
	ringBuffer.front(strLen, res);
	ringBuffer.pop(strLen);
	printf("%s\n", res);

	ringBuffer.push(strLen, str);
	ringBuffer.front(strLen, res);
	ringBuffer.pop(strLen);
	printf("%s\n", res);

	ringBuffer.push(strLen, str);
	ringBuffer.front(strLen, res);
	ringBuffer.pop(strLen);
	printf("%s\n", res);

	ringBuffer.push(strLen, str);
	ringBuffer.front(strLen, res);
	ringBuffer.pop(strLen);
	printf("%s\n", res);

	ringBuffer.push(strLen, str);
	ringBuffer.front(strLen, res);
	ringBuffer.pop(strLen);
	printf("%s\n", res);
	return 0;
}