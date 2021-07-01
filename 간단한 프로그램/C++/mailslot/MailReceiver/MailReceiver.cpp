
#include <stdio.h>
#include <windows.h>
#include <string.h>

#define SLOT_NAME L"\\\\.\\mailslot\\mailbox.txt"

int main(int argc, WCHAR* argv[]) {

	HANDLE hMailSlot;
	WCHAR msgBox[50];
	DWORD bytesRead;

	hMailSlot = CreateMailslotW(SLOT_NAME, 0, MAILSLOT_WAIT_FOREVER, NULL);
	if (hMailSlot == INVALID_HANDLE_VALUE) {
		DWORD error = GetLastError();
		wprintf(L"code: %d\nUnable to create mailslot", error);
		system("PAUSE>NUL");
		return 1;
	}

	wprintf(L"************** msg ***************\n");

	for (;;) {

		if (!ReadFile(hMailSlot, msgBox, sizeof(WCHAR) * 50, &bytesRead, NULL)) {
			DWORD error = GetLastError();
			wprintf(L"code: %d\nUnable to read", error);
			system("PAUSE>NUL");
			return 1;
		}

		msgBox[bytesRead / sizeof(WCHAR)] = 0;
		if (!wcscmp(msgBox, L"exit")) {
			wprintf(L"exit");
			break;
		}

		wprintf(L"%s\n", msgBox);
	}

	CloseHandle(hMailSlot);

	return 0;
}