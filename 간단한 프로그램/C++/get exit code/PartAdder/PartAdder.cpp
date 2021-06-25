#include <stdio.h>
#include <Windows.h>

int wmain(int argc, WCHAR* argv[]) {

	if (argc != 3) {
		return -1;
	}

	DWORD start = _wtoi(argv[1]);
	DWORD end = _wtoi(argv[2]);

	DWORD total = 0;

	for (DWORD numCnt = start; numCnt <= end; numCnt++) {
		total += numCnt;
	}

	return (int)total;
}

