#include <stdio.h>
#include <Windows.h>

int wmain(int argc, WCHAR* argv[]) {

	HANDLE hMailSlot;
	WCHAR msg[50];
	DWORD bytesWritten;

	FILE* file; 
	_wfopen_s(&file, L"handle.txt", L"rb");
	if (file == nullptr) {
		return 0;
	}
	fread(&hMailSlot, sizeof(HANDLE), 1, file);
	fclose(file);

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

	return 0;
}