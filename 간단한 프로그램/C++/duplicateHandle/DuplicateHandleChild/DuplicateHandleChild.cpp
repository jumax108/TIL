#include <stdio.h>
#include <windows.h>

int wmain(int argc, WCHAR* argv[]) {

	HANDLE hParent = (HANDLE)_wtoi(argv[1]);

	DWORD isSuccess = WaitForSingleObject(hParent, INFINITE);
	if (isSuccess == WAIT_FAILED) {
		DWORD err = GetLastError();
		wprintf(L"error code = %d", err);
		system("PAUSE>NUL");
		return -1;
	}

	wprintf(L"[child start]\n");

	wprintf(L"hahaha\n");

	wprintf(L"[child end]\n");

	

	system("PAUSE>NUL");
	return 0;

}