#include <stdio.h>
#include <windows.h>

int wmain(int argc, WCHAR* argv[]) {

	wprintf(L"[parent start]\n");

	HANDLE hProcess;
	WCHAR cmd[1024];

	DuplicateHandle(
		GetCurrentProcess(), GetCurrentProcess(),
		GetCurrentProcess(), &hProcess, 0, true, DUPLICATE_SAME_ACCESS);

	swprintf_s(cmd, L"%s %u", L"DuplicateHandleChild.exe", (ULONG)hProcess);

	STARTUPINFO si = { 0, };
	PROCESS_INFORMATION pi = { 0, };
	si.cb = sizeof(si);

	bool isSuccess = CreateProcessW(NULL, cmd, NULL, NULL, true, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if (isSuccess == false) {
		DWORD err = GetLastError();
		wprintf(L"error code = %d\n프로세스 생성 실패", err);
		system("PAUSE>NUL");
		return -1;
	}

	CloseHandle(hProcess);

	wprintf(L"[parent end]\n");

	return 0;

}