#include <stdio.h>
#include <Windows.h>
#include <process.h>

unsigned int WINAPI ThreadProc(LPVOID lpParameter) {

	DWORD* ptr = (DWORD*)lpParameter;

	DWORD startNum = *ptr;
	DWORD endNum = *(ptr + 1);

	for (DWORD numberCnt = startNum; numberCnt <= endNum; ++numberCnt) {
		if (numberCnt > 2 && numberCnt % 2 == 0) {
			continue;
		}
		DWORD divCnt = 2;
		for (; divCnt < numberCnt / 2; ++divCnt) {

			if (numberCnt % divCnt == 0) {
				break;
			}

		}
		if (divCnt == numberCnt / 2) {

			printf("prime: %d\n", numberCnt);

		}

	}

	return 0;
}



int main() {

	DWORD val[] = { 1, 500, 501, 1000, 1001, 1500};

	constexpr DWORD THREAD_NUM = 3;

	HANDLE threadHandle[THREAD_NUM];
	unsigned int threadId[THREAD_NUM];
	ZeroMemory(threadHandle, sizeof(HANDLE) * THREAD_NUM);
	ZeroMemory(threadId, sizeof(DWORD) * THREAD_NUM);

	for (int threadCnt = 0; threadCnt < THREAD_NUM; threadCnt++) {\
		
		threadHandle[threadCnt] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, &val[threadCnt * 2], 0, &threadId[threadCnt]);
		if (threadHandle[threadCnt] == nullptr) {
			DWORD code = GetLastError();
			printf("Thread Create Error\ncode: %d\n", code);
			return 0;
		}
	}

	WaitForMultipleObjects(THREAD_NUM, threadHandle, true, INFINITE);

	for (int threadCnt = 0; threadCnt < THREAD_NUM; ++threadCnt) {

		HANDLE handle = threadHandle[threadCnt];

		if (handle != nullptr) {
			DWORD result = CloseHandle(handle);
			if (result == NULL) { 
				DWORD code = GetLastError();
				printf("Close Handle Error\ncode: %d\n", code);
				return 0;
			}
		}
	}

	return 0;
}