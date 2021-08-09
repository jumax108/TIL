#include <stdio.h>
#include <Windows.h>
#include <locale.h>
#include <exception>

#include "queue.h"

const WCHAR* pass = L"pass";
const WCHAR* fail = L"fail";

int main() {

	setlocale(LC_ALL, "ko-KR");
	FILE* resultFile;
	_wfopen_s(&resultFile, L"result.txt", L"w");
	int fopenError;
	if (resultFile == 0) {
		fopenError = GetLastError();
		wprintf(L"result file open error : %d\n\n", fopenError);
		return -1;
	}

	{
		wprintf(L"\n 현재 size가 정확하게 return 되는가 \n\n");
		fwprintf(resultFile, L"\n 현재 size가 정확하게 return 되는가 \n\n");

		CQueue<int> size(10);
		int currentSize = size.size();

		const WCHAR* result = (currentSize == 0 ? pass : fail);
		wprintf(L"기대 size : 0, 결과 size : %d, 결과 : %s\n", currentSize, result);
		fwprintf(resultFile, L"기대 size : 0, 결과 size : %d, 결과 : %s\n", currentSize, result);

		for (int numCnt = 1; numCnt <= 10; numCnt++) {

			size.push(0);
			currentSize = size.size();
			if (currentSize == numCnt) {
				result = pass;
			}
			else {
				result = fail;
			}
			wprintf(L"기대 size : %d, 결과 size : %d, 결과 : %s\n", numCnt, currentSize, result);
			fwprintf(resultFile, L"기대 size : %d, 결과 size : %d, 결과 : %s\n", numCnt, currentSize, result);

		}
	}

	{
		wprintf(L"\n 설정한 capacity가 나오는가 \n\n");
		fwprintf(resultFile, L"\n 설정한 capacity가 나오는가 \n\n");

		size_t setCap[5] = { 1, 5415, 518948, 9847984, 42949672 };
		CQueue<int>* capQue;
		const WCHAR* result;

		for (int capCnt = 0; capCnt < 5; capCnt++) {

			capQue = new CQueue<int>(setCap[capCnt]);

			UINT cap = capQue->capacity();
			if (cap == setCap[capCnt]) {
				result = pass;
			}
			else {
				result = fail;
			}
			wprintf(L"기대 size : %d, 결과 size : %d, 결과 : %s\n", setCap[capCnt], cap, result);
			fwprintf(resultFile, L"기대 size : %d, 결과 size : %d, 결과 : %s\n", setCap[capCnt], cap, result);

			delete(capQue);
		}
	}

	{
		wprintf(L"\n 빈 공간이 있으면 데이터를 push 할 수 있는가\n");
		fwprintf(resultFile, L"\n 빈 공간이 있으면 데이터를 push 할 수 있는가\n");
		wprintf(L" 빈 공간이 없으면 데이터를 push 할 수 없는가\n\n");
		fwprintf(resultFile, L" 빈 공간이 없으면 데이터를 push 할 수 없는가\n\n");

		CQueue<int> push(5);
		bool result[6] = { true, true, true, true, true, false };
		WCHAR* resultText;
		for (int numCnt = 0; numCnt < 6; ++numCnt) {

			bool pushResult;
			pushResult = push.push(numCnt);
			if (pushResult == result[numCnt]) {
				resultText = (WCHAR*)pass;
			}
			else {
				resultText = (WCHAR*)fail;
			}

			wprintf(L"기대 값 : %d, 결과 값 : %d, 결과 : %s\n", result[numCnt], pushResult, resultText);
			fwprintf(resultFile, L"기대 값 : %d, 결과 값 : %d, 결과 : %s\n", result[numCnt], pushResult, resultText);

		}

	}

	{

		wprintf(L"\n 데이터가 없으면 데이터를 pop 할 수 없다\n");
		fwprintf(resultFile, L"\n 데이터가 없으면 데이터를 pop 할 수 없다\n");
		wprintf(L" 데이터가 있으면 데이터를 pop 할 수 있다.\n\n");
		fwprintf(resultFile, L" 데이터가 있으면 데이터를 pop 할 수 있다.\n\n");

		CQueue<int> pop(5);
		bool result[6] = { true, true, true, true, true, false };
		WCHAR* resultText;

		for (int numCnt = 0; numCnt < 5; ++numCnt) {
			pop.push(numCnt);
		}

		for (int numCnt = 0; numCnt < 6; ++numCnt) {

			bool pushResult;
			pushResult = pop.pop();
			if (pushResult == result[numCnt]) {
				resultText = (WCHAR*)pass;
			}
			else {
				resultText = (WCHAR*)fail;
			}

			wprintf(L"기대 값 : %d, 결과 값 : %d, 결과 : %s\n", result[numCnt], pushResult, resultText);
			fwprintf(resultFile, L"기대 값 : %d, 결과 값 : %d, 결과 : %s\n", result[numCnt], pushResult, resultText);
		}
	}

	{

		wprintf(L"\n 데이터가 없으면 front 할 수 없다.\n");
		fwprintf(resultFile, L"\n 데이터가 없으면 front 할 수 없다.\n");
		wprintf(L" 데이터가 있으면 맨 앞의 데이터가 획득된다.\n\n");
		fwprintf(resultFile, L" 데이터가 있으면 맨 앞의 데이터가 획득된다.\n\n");

		CQueue<int> front(5);
		int pushData[6] = { 1, 2, 3, 4, 5, 6 };
		bool doPop[6] = { true, false, true, false, true, false };
		
		int outData[6] = { 0, 2, 3, 3, 4, 4 };
		bool returnData[6] = { false, true, true, true, true, true };

		WCHAR* result;

		for (int tcCnt = 0; tcCnt < 6; tcCnt++) {

			front.push(pushData[tcCnt]);
			if (doPop[tcCnt] == true) {
				front.pop();
			}

			int out = 0;
			bool frontResult = front.front(&out);
			
			if (returnData[tcCnt] == frontResult && outData[tcCnt] == out) {

				result = (WCHAR*)pass;

			}
			else {

				result = (WCHAR*)fail;

			}

			wprintf(L"기대 반환값: %d, 기대 데이터: %d || 실제 반환값: %d, 실제 데이터: %d || 결과: %s\n", returnData[tcCnt], outData[tcCnt], frontResult, out, result);
			fwprintf(resultFile, L"기대 반환값: %d, 기대 데이터: %d || 실제 반환값: %d, 실제 데이터: %d || 결과: %s\n", returnData[tcCnt], outData[tcCnt], frontResult, out, result);

		}

	}

	return 0;

}