#include <windows.h>
#include <conio.h>
#include <thread>
#include <locale>
#include <list>

#include "ringBuffer.h"

CRingBuffer msgBuffer(500);

const wchar_t* orgStr = L"Nintendo64";
const unsigned int strLen = wcslen(orgStr);

struct stMsgHead {
	unsigned short type;
	unsigned short size;
};

SRWLOCK strListLock;
std::list<wchar_t*> strList;

unsigned int __stdcall workerThreadProc (void*);

CRITICAL_SECTION printCriticalSection;

enum class MESSAGE_TYPE {
	ADD_STR = 0,
	DEL_STR,
	PRINT,
	EXIT
};

int maxStrNum = 0;


int main() {

	setlocale(LC_ALL,"");

	InitializeCriticalSection(&printCriticalSection);

	constexpr unsigned int seed = 0;
	srand(seed);

	constexpr unsigned short THREAD_NUM = 3;
	HANDLE workerThread[THREAD_NUM] = {0,};
	const HANDLE* const workerThreadEnd = workerThread + THREAD_NUM;

	int threadProcessCnt[THREAD_NUM] = {0, };

	HANDLE jobPushed = CreateEvent(nullptr, false, false, nullptr);

	InitializeSRWLock(&strListLock);

	// 워커 스레드 초기화
	void* argList[THREAD_NUM][2];
	{
		int* threadProcessCntIter = threadProcessCnt;
		for (int threadCnt = 0; threadCnt < THREAD_NUM; ++threadCnt) {
			argList[threadCnt][0] = jobPushed;
			argList[threadCnt][1] = threadProcessCntIter++;
			workerThread[threadCnt] = (HANDLE)_beginthreadex(nullptr, 0, workerThreadProc, argList[threadCnt], 0, nullptr);
		}
	}

	// 구분선 출력
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 5 });
		wprintf(L"────────────────────────────────────\n");
	}
	// process
	{
		unsigned int printTime = 0;

		while (_kbhit() == NULL || _getch() != ' ') {

			unsigned short type = rand() % 3;
			unsigned short size = 0;
			wchar_t* subStr = nullptr;

			stMsgHead head;
			head.type = type;
			head.size = size;

			if (type == (unsigned short)MESSAGE_TYPE::ADD_STR) {
				size = rand() % strLen + 1;
				head.size = size;
			}

			if(msgBuffer.getFreeSize() < sizeof(stMsgHead) + size * sizeof(wchar_t)){
				continue;
			}

			msgBuffer.push(sizeof(stMsgHead), &head);
			if (type == (unsigned short)MESSAGE_TYPE::ADD_STR) {
				msgBuffer.push(size * sizeof(wchar_t), orgStr);
			}

			SetEvent(jobPushed);
			
			// 출력
			unsigned int checkTime = GetTickCount();
			if(checkTime - printTime >= 1000){
				printTime = checkTime;
				EnterCriticalSection(&printCriticalSection); {
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });
					wprintf(L"메시지 큐 남은량: %4d\n", msgBuffer.getFreeSize());
					wprintf(L"메시지 큐 사용량: %4d\n", msgBuffer.getUsedSize());
					for(int threadCnt = 0; threadCnt < THREAD_NUM; threadCnt++){
						wprintf(L"Thread %2d: %4d\n",threadCnt, threadProcessCnt[threadCnt]);
						threadProcessCnt[threadCnt] = 0;
					}
				}LeaveCriticalSection(&printCriticalSection);
			}

		}
	}

	// 스레드에 종료 메시지 전달
	{
		stMsgHead head;
		head.type = (unsigned short)MESSAGE_TYPE::EXIT;
		head.size = 0;

		for (int threadCnt = 0; threadCnt < THREAD_NUM; ++threadCnt) {
			msgBuffer.push(sizeof(stMsgHead), &head);
		}
	}

	// 워커 스레드 종료 대기
	{
		WaitForMultipleObjects(THREAD_NUM, workerThread, true, INFINITE);
	}

	// 워커 스레드 정리
	{
		for (HANDLE* workerThreadIter = workerThread; workerThreadIter != workerThreadEnd; ++workerThreadIter) {
			CloseHandle(*workerThreadIter);
		}
	}

	return 0;
}

unsigned int __stdcall workerThreadProc(void* arg) {

	HANDLE waitJob = ((HANDLE*)arg)[0];
	int* processCnt = ((int**)arg)[1];

	for (;;) {

		WaitForSingleObject(waitJob, INFINITE);

		do{

			stMsgHead head;
			wchar_t* subStr = nullptr;

			// get msg
			bool recvFail = false;
			msgBuffer.popLock();
			do{

				bool getData = msgBuffer.front(sizeof(stMsgHead), &head);
				if(getData == false){
				    // while 조건에 동시에 달성했지만 다른 스레드에서 먼저 빼감
					recvFail = true;
					break;
				}
				if (msgBuffer.getUsedSize() < sizeof(stMsgHead) + head.size * sizeof(wchar_t)) {
					// 메시지 완성 안됨
					recvFail = true;
					break;
				}

				msgBuffer.pop(sizeof(stMsgHead));

				if (head.type == (unsigned short)MESSAGE_TYPE::ADD_STR) {
					subStr = new wchar_t[head.size + 1];
					ZeroMemory(subStr, (head.size + 1) * sizeof(wchar_t));					
					msgBuffer.front(head.size * sizeof(wchar_t), subStr);
					msgBuffer.pop(head.size * sizeof(wchar_t));
				}

			}while(false);
			msgBuffer.popUnlock();

			if(recvFail == true){
				// 메시지 정상적으로 못받았으면 프로세스 돌지 않음
				break;
			}

			// proc msg
			{
				switch ((MESSAGE_TYPE)head.type) {
				
					case MESSAGE_TYPE::ADD_STR: {
						AcquireSRWLockExclusive(&strListLock);
						// 추가할 때 연결을 끊어버리는지 size 보다 항목이 적은 경우가 있음
						// 그래서 shared로 안함
						strList.push_back(subStr);
						maxStrNum = max(maxStrNum, strList.size());

						ReleaseSRWLockExclusive(&strListLock);
					}
					break;

					case MESSAGE_TYPE::DEL_STR:{
						AcquireSRWLockExclusive(&strListLock);
						do{
							if(strList.size() == 0){
								break;
							}
							delete[] strList.back();
 							strList.pop_back();
						}while(false);
						ReleaseSRWLockExclusive(&strListLock);
					}
					break;

					case MESSAGE_TYPE::PRINT: {

						unsigned __int64 listSize = strList.size();
						size_t printStrLen = (listSize / 10 + 1) * 150;
						wchar_t* printStr = new wchar_t[printStrLen + 1];
						printStr[printStrLen] = '\0';

						wmemset(printStr, ' ',printStrLen);
						swprintf_s(printStr, printStrLen, L"%10s",L"list: ");

						
						AcquireSRWLockShared(&strListLock);
						{
							std::list<wchar_t*>::iterator strIter = strList.begin();
							size_t writeByte=0;
							for(int listCnt = 0; listCnt < listSize && strIter != strList.end() ; ++listCnt, ++strIter){
								writeByte = swprintf_s(printStr, printStrLen, L"%s[%10s] ",printStr, *strIter);
								if((listCnt + 1) % 10 == 0){
									writeByte = swprintf_s(printStr, printStrLen, L"%s\n%10s", printStr,L"");
								}
							}
							printStr[writeByte] = ' ';
						} ReleaseSRWLockShared(&strListLock);

						wchar_t printSpace[150];
						printSpace[149] = '\0';
						wmemset(printSpace,' ', 149);
						int lineNum = (maxStrNum / 10 + 1);
						EnterCriticalSection(&printCriticalSection);{
							SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {0,6});
							wprintf_s(L"%s\n", printStr);
							for(int i=0 ; i<lineNum ; i++){
							wprintf_s(L"%s\n", printSpace);
							}
						} LeaveCriticalSection(&printCriticalSection);



						delete[] printStr;
					}
					break;

					case MESSAGE_TYPE::EXIT: {

						return 0;
					}
					break;

				}
			}
			*processCnt += 1;
		}while(msgBuffer.getUsedSize() > 0);

	}

	return 0;
}