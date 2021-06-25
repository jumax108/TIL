#include <stdio.h>
#include <Windows.h>

int processCall(WCHAR command[]) {
	
	STARTUPINFOW si;
	ZeroMemory(&si, sizeof(STARTUPINFO));

	PROCESS_INFORMATION pi;
	
	int result = 0;

	si.cb = sizeof(STARTUPINFO);
	CreateProcessW(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	//int error = GetLastError();

	WaitForSingleObject(pi.hProcess, INFINITE);

	GetExitCodeProcess(pi.hProcess, (LPDWORD)&result);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	
	return result;

}

int wmain(int argc, WCHAR* argv[]) {

	int sum = 0;
	int val = 0;
	WCHAR command[] = L"PartAdder.exe 1 5";
	val = processCall(command);
	if (val == -1) {
		return -1;
	}
	sum += val;

	wsprintfW(command, L"PartAdder.exe 6 10");
	val = processCall(command);
	if (val == -1) {
		return -1;
	}
	sum += val;

	wprintf_s(L"%d", sum);

	return 0;
}