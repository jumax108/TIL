#include <stdio.h>
#include <Windows.h>

int wmain(int argc, WCHAR* argv[]) {

	SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);

	for (;;) {

		for (DWORD i = 0; i < 100000000; i++);

		wprintf(L"BELOW_NORMAL_PRIORITY_CLASS Process\n");

	}

	return 0;

}