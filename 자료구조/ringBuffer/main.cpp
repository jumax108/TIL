#include <stdio.h>
#include <Windows.h>
#include "ringBuffer.h"

int main() {

	CRingBuffer ringBuffer(10);

	const BYTE* str = (const BYTE*)"Hello";
	const BYTE* str2 = (const BYTE*)"World";
	const BYTE* str3 = (const BYTE*)"hahahaha";

	BYTE* res = (BYTE*)malloc(sizeof(BYTE) * 100);
	ZeroMemory(res, 100);

	ringBuffer.push(strlen((const char*)str), str);
	ringBuffer.pop(4);
	ringBuffer.push(strlen((const char*)str2), str2);
	ringBuffer.pop(4);
	ringBuffer.push(strlen((const char*)str3), str3);

	ringBuffer.front(10, res);

	printf("%s\n", res);

	return 0;
}