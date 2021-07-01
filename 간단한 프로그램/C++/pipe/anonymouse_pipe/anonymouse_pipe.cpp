#include <stdio.h>
#include <windows.h>
#include <string.h>

int wmain(int argc, WCHAR* argv[]) {

	WCHAR* sendStr = (WCHAR*)L"anonymous pipe";
	WCHAR* recvStr = (WCHAR*)malloc(100 * sizeof(WCHAR));
	
	DWORD bytesWritten;
	DWORD bytesRead;

	HANDLE hReadPipe, hWritePipe;
	CreatePipe(&hReadPipe, &hWritePipe, NULL, 0);
	WriteFile(hWritePipe, sendStr, wcslen(sendStr) * sizeof(WCHAR), &bytesWritten, NULL);
	ReadFile(hReadPipe, recvStr, bytesWritten, &bytesRead, NULL);

	recvStr[bytesRead / sizeof(WCHAR)] = '\0';
	wprintf(L"str: %s", recvStr);

	CloseHandle(hReadPipe);
	CloseHandle(hWritePipe);

	return 0;
}