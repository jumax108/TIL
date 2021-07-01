#include <stdio.h>
#include <Windows.h>

constexpr WCHAR* SLOT_NAME = (WCHAR*)L"\\\\.\\mailslot\\mailbox.txt";

int main(int argc, WCHAR* argv[]) {
	HANDLE hMailSlot;
	WCHAR msg[50];
	DWORD bytesWritten;

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = true;
	sa.lpSecurityDescriptor = NULL;
	hMailSlot = CreateFileW(SLOT_NAME, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	wprintf(L"%s\n", SLOT_NAME);

	if (hMailSlot == INVALID_HANDLE_VALUE) {
		DWORD error = GetLastError();
		wprintf(L"error = %d\nUnable to Create MailSlot", error);
		system("PAUSE>NUL");
		return 1;
	}

	FILE* file;
	_wfopen_s(&file, L"handle.txt", L"wb");
	if (file == nullptr) {
		system("PAUSE>NUL");
		return 1;
	}
	fwrite(&hMailSlot, sizeof(HANDLE), 1, file);
	fclose(file);

	STARTUPINFO si = { 0, };
	PROCESS_INFORMATION pi = {0,};
	si.cb = sizeof(STARTUPINFO);
	WCHAR command[] = L"MailSender2.exe";
	CreateProcessW(nullptr, command, NULL, NULL, true, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	
	while (1) {

		wprintf(L"Send Msg > ");
		wscanf_s(L"%s", msg);

		if (!WriteFile(hMailSlot, msg, wcslen(msg) * sizeof(WCHAR), &bytesWritten, NULL)) {

			DWORD error = GetLastError();
			wprintf(L"error = %d\nUnable to write!", error);
			break;
		}

		if (!wcscmp(msg, L"exit")) {
			wprintf(L"exit");
			break;
		}

	}

	CloseHandle(hMailSlot);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}