#include <stdio.h>
#include <Windows.h>
#include <thread>
#include <time.h>

#define TEST


//#define INTERLOCK
//#define CRITICALSECTION
#define SRW

int data;
int connectNum;
bool beShutdown;

unsigned int __stdcall acceptFunc(void*);
unsigned int __stdcall disconnectFunc(void*);

#if defined(INTERLOCK)
	unsigned int __stdcall updateInterlockFunc(void*);
#elif defined(CRITICALSECTION)
	CRITICAL_SECTION dataCriticalSection;
	unsigned int __stdcall updateCriticalSectionFunc(void*);
#elif defined(SRW)
	SRWLOCK dataSrwLock;
	unsigned int __stdcall updateSrwFunc(void*);
#endif

#ifdef TEST
	int acceptCall;
	int disconnectCall;
#endif

int main() {

	srand(0);

	constexpr int UPDATE_THREAD_NUM = 3;
	constexpr int THREAD_NUM = UPDATE_THREAD_NUM + 2;
	constexpr int TARGET_PROC_SEC = 20;

	#ifdef TEST
		int* updateCall = new int[UPDATE_THREAD_NUM];
		ZeroMemory(updateCall, sizeof(int) * UPDATE_THREAD_NUM);
	#endif

	// accept Thread 생성
	HANDLE acceptThread;
	{
		acceptThread = (HANDLE)_beginthreadex(nullptr, 0, acceptFunc, nullptr, 0, nullptr);
	}

	// disconnect Thread 생성
	HANDLE disconnectThread;
	{
		disconnectThread = (HANDLE)_beginthreadex(nullptr, 0, disconnectFunc, nullptr, 0, nullptr);
	}

	// update Thread 생성
	HANDLE updateThread[UPDATE_THREAD_NUM];
	{
		unsigned int(__stdcall * updateFunc) (void*);
		#if defined(INTERLOCK)
			updateFunc = updateInterlockFunc;
		#elif defined(CRITICALSECTION)
			InitializeCriticalSection(&dataCriticalSection);
			updateFunc = updateCriticalSectionFunc;
		#elif defined(SRW)
			InitializeSRWLock(&dataSrwLock);
			updateFunc = updateSrwFunc;
		#endif

		HANDLE* updateThreadEnd = updateThread + UPDATE_THREAD_NUM;
		for (HANDLE* updateThreadIter = updateThread; updateThreadIter != updateThreadEnd; ++updateThreadIter) {
			#ifdef TEST
				*updateThreadIter = (HANDLE)_beginthreadex(nullptr, 0, updateFunc, updateCall++, 0, nullptr);
			#else
				*updateThreadIter = (HANDLE)_beginthreadex(nullptr, 0, updateFunc, nullptr, 0, nullptr);
			#endif
		}
	}

	// 1초마다 data 값 출력
	{
		for (int curProcSec = 0; curProcSec < TARGET_PROC_SEC; ++curProcSec) {

			wprintf(L"%10s: %10s: %4d\n", L"main", L"connect", connectNum);
			Sleep(999);

		}
	}

	#ifdef TEST
		wprintf(L"---------------------------\n");
	#endif

	// 스레드 종료 유도
	{
		beShutdown = true;
	}

	// 스레드 종료 대기
	HANDLE threads[THREAD_NUM];
	{
		for (int threadCnt = 0; threadCnt < UPDATE_THREAD_NUM; ++threadCnt) {
			threads[threadCnt] = updateThread[threadCnt];
		}
		threads[UPDATE_THREAD_NUM] = acceptThread;
		threads[UPDATE_THREAD_NUM + 1] = disconnectThread;

		WaitForMultipleObjects(THREAD_NUM, threads, true, INFINITE);
	}

	// 스레드 정리
	{
		HANDLE* threadsEnd = threads + THREAD_NUM;
		for (HANDLE* threadIter = threads; threadIter != threadsEnd; ++threadIter) {
			CloseHandle(*threadIter);
		}
	}

	wprintf(L"%10s: %10s: %4d\n", L"main", L"data", data);

	return 0;
}

unsigned int __stdcall acceptFunc(void* argList) {
	
	for (;;) {

		if (beShutdown == true) {
			break;
		}

		InterlockedIncrement((LONG*)&connectNum);

		#ifdef TEST
			acceptCall += 1;
		#endif

		Sleep(rand() % 901 + 100 - 1);
	}

	#ifdef TEST
		wprintf(L"%10s: %10s: %4d\n", L"accept", L"call", acceptCall);
	#endif

	return 0;
}

unsigned int __stdcall disconnectFunc(void* argList) {

	for (;;) {

		if (beShutdown == true) {
			break;
		}

		InterlockedDecrement((LONG*)&connectNum);

		#ifdef TEST
			disconnectCall += 1;
		#endif

		Sleep(rand() % 901 + 100 - 1);
	}

	#ifdef TEST
		wprintf(L"%10s: %10s: %4d\n",L"disconnect", L"call", disconnectCall);
	#endif

	return 0;
}

#ifdef INTERLOCK
unsigned int __stdcall updateInterlockFunc(void* arg) {

	#ifdef TEST
		srand((unsigned int)arg);
	#endif

	for (;;) {

		if (beShutdown == true) {
			break;
		}

		long addResult = InterlockedIncrement((long*)&data);
		if (addResult % 1000 == 0) {
			wprintf(L"%10s: %10s: %4d\n", L"updateFunc", L"data", addResult);
		}

		#ifdef TEST
			*(int*)arg += 1;
			Sleep(rand() % 5);
		#else
			Sleep(10 - 1);
		#endif
	}

	#ifdef TEST
		wprintf(L"%10s: %10s: %4d\n",L"update", L"call", *(int*)arg);
	#endif

	return 0;
}
#endif

#ifdef CRITICALSECTION
unsigned int __stdcall updateCriticalSectionFunc(void* arg) {
	#ifdef TEST
		srand((unsigned int)arg);
	#endif

	for (;;) {

		if (beShutdown == true) {
			break;
		}

		EnterCriticalSection(&dataCriticalSection); 
		{
			data += 1;
			if (data % 1000 == 0) {
				wprintf(L"%10s: %10s: %4d\n", L"updateFunc", L"data", data);
			}

		} LeaveCriticalSection(&dataCriticalSection);

		#ifdef TEST
			* (int*)arg += 1;
			Sleep(rand() % 5);
		#else
			Sleep(10 - 1);
		#endif
	}

	#ifdef TEST
		wprintf(L"%10s: %10s: %4d\n", L"update", L"call", *(int*)arg);
	#endif
	return 0;
}
#endif

#ifdef SRW
unsigned int __stdcall updateSrwFunc(void* arg) {
	
	#ifdef TEST
		srand((unsigned int)arg);
	#endif

	for (;;) {

		if (beShutdown == true) {
			break;
		}

		AcquireSRWLockExclusive(&dataSrwLock);
		{
			data += 1;
			if (data % 1000 == 0) {
				wprintf(L"%10s: %10s: %4d\n", L"updateFunc", L"data", data);
			}

		}ReleaseSRWLockExclusive(&dataSrwLock);


		#ifdef TEST
			* (int*)arg += 1;
			Sleep(rand() % 5);
		#else
			Sleep(10 - 1);
		#endif

	}

	#ifdef TEST
		wprintf(L"%10s: %10s: %4d\n", L"update", L"call", *(int*)arg);
	#endif

	return 0;
}
#endif