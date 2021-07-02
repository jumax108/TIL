#include <stdio.h>
#include <Windows.h>

int wmain(int argc, WCHAR* argv[]) {

	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);

	for (;;) {

		for (DWORD i = 0; i < 100000000; i++) {}

		wprintf(L"NORMAL_PRIORITY_CLASS\n");

	}

	return 0;
}