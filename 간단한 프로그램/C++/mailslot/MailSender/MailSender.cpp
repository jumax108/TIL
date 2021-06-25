#include <stdio.h>
#include <Windows.h>

#define SLOT_NAME L"\\\\.\\mailslot\\mailbox"

int main(int argc, WCHAR* argv[]) {
	HANDLE hMailSlot;
	WCHAR msg[50];
	DWORD bytesWritten;

	hMailSlot = CreateFile(SLOT_NAME, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hMailSlot == INVALID_HANDLE_VALUE) {
		DWORD error = GetLastError();
		wprintf(L"error = %d\nUnable to Create MailSlot", error);
		return 1;
	}

	while (1) {

		wprintf(L"Send Msg > ");
		wscanf_s(L"%s", msg);

		if (!WriteFile(hMailSlot, msg, wcslen(msg) * sizeof(WCHAR), &bytesWritten, NULL)) {

			DWORD error = GetLastError();
			wprintf(L"error = %d\nUnable to write!", error);
			CloseHandle(hMailSlot);
			return 1;
		}

		if (!wcscmp(msg, L"exit")) {
			wprintf(L"exit");
			break;
		}

	}

	CloseHandle(hMailSlot);
	return 0;
}