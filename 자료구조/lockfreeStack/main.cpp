#include <Windows.h>
#include <stdio.h>
#include <thread>
#include <list>
#include <Dbghelp.h>
#include <conio.h>

#include "UnsafeObjectFreeList.h"
#include "lockFreeStack.h"
#include "dump.h"

int size = 0;

struct stNode{

	int data = 0;
	bool use;
};
HANDLE _printTpsThread;

CLockFreeStack<stNode*> stack;

CDump dump;

unsigned int tps = 0;

constexpr int THREAD_NUM = 3;
constexpr int NODE_EACH_THREAD = 1000;
constexpr int MAX_NODE = THREAD_NUM * NODE_EACH_THREAD;

stNode _numArr[MAX_NODE];

HANDLE _logicThread[THREAD_NUM];

HANDLE _threadExitEvent;

unsigned __stdcall logicTestFunc(void*);
unsigned __stdcall printTPS(void*);

int main(){

	_threadExitEvent = CreateEventW(nullptr, true, false, L"threadExit");

	_printTpsThread = (HANDLE)_beginthreadex(nullptr, 0, printTPS, nullptr, 0, nullptr);

	for(int threadCnt = 0; threadCnt < THREAD_NUM; ++threadCnt){

		_logicThread[threadCnt] = (HANDLE)_beginthreadex(nullptr, 0, logicTestFunc, &_logicThread[threadCnt], 0, nullptr);

	}

	while(_getwch() != L'q');

	SetEvent(_threadExitEvent);

	HANDLE* allThread = new HANDLE[THREAD_NUM + 1];
	
	for(int threadCnt = 0; threadCnt < THREAD_NUM ; ++threadCnt){

		allThread[threadCnt] = _logicThread[threadCnt];

	}
	allThread[THREAD_NUM] = _printTpsThread;

	WaitForMultipleObjects(THREAD_NUM + 1, allThread, true, INFINITE);

	delete[] allThread;

	return 0;

}

unsigned __stdcall logicTestFunc(void* arg){

	///
	/// 
	/// �׽�Ʈ �ó�����
	/// 
	/// 1. nodeNum ��ŭ�� ��带 ���ÿ� push �Ѵ�.
	/// 2. ���ÿ��� nodeNum ��ŭ�� ��带 pop �Ѵ�.
	/// 3. ����� �����Ͱ� �ʱ� ���������� Ȯ���Ѵ�.					-> �ٸ� �����忡�� ����ߴ��� Ȯ��
	/// 4. ����� �����͸� 1 �����Ѵ�.
	/// 5. ����� �����Ͱ� 1 ���� Ȯ���Ѵ�.								-> �ٸ� �����忡�� ����ߴ��� Ȯ��
	/// 6. ����� �����͸� 1 �����Ѵ�.
	/// 7. ����� �����Ͱ� 0 ���� Ȯ���Ѵ�.								-> �ٸ� �����忡�� ����ߴ��� Ȯ��
	/// 8. �ݺ��Ѵ�.
	/// 

	HANDLE threadHandle = *(HANDLE*)arg;
	int nodeNum = NODE_EACH_THREAD;
	stNode** arr = new stNode*[nodeNum];
	
	for(;;){

		unsigned int threadExit = WaitForSingleObject(_threadExitEvent, 0);
		if(threadExit == WAIT_OBJECT_0){
			break;
		}

		int indexCnt = 0;
		for(int nodeCnt = 0; nodeCnt < nodeNum; ++indexCnt){

			bool used = InterlockedExchange8((char*)&_numArr[indexCnt].use, true);
			if(used == true){
				continue;
			}

			nodeCnt += 1;
			stNode* node = &_numArr[indexCnt];

			bool pushResult = stack.push(node, threadHandle);
			
			if(pushResult == false){
				// error
				CDump::crash();
			}
		}

		for(int nodeCnt = 0; nodeCnt < nodeNum; ++nodeCnt){

			bool popResult = stack.pop(&arr[nodeCnt], threadHandle);
						
			stNode* node = arr[nodeCnt];

			if(popResult == false){
				// error
				CDump::crash();
			}
		}

		for(int nodeCnt = 0; nodeCnt < nodeNum; ++nodeCnt){
			if(arr[nodeCnt]->data != 0){
				// error
				CDump::crash();
			}
		}

		for(int nodeCnt = 0; nodeCnt < nodeNum; ++nodeCnt){
		
			InterlockedIncrement((unsigned int*)&arr[nodeCnt]->data);
			
			stNode* node = arr[nodeCnt];
			
		}

		
		for(int nodeCnt = 0; nodeCnt < nodeNum; ++nodeCnt){
			if(arr[nodeCnt]->data != 1){
				// error
				CDump::crash();
			}
		}

		for(int nodeCnt = 0; nodeCnt < nodeNum; ++nodeCnt){
		
			stNode* node = arr[nodeCnt];
			InterlockedDecrement((unsigned int*)&node->data);
			
			
		}
		
		for(int nodeCnt = 0; nodeCnt < nodeNum; ++nodeCnt){
			if(arr[nodeCnt]->data  != 0){
				// error
				CDump::crash();
			}
			arr[nodeCnt]->use = false;
		}

		InterlockedIncrement(&tps);

	}

	delete[] (arr);

	return 0;
}

unsigned __stdcall printTPS(void*){

	while(1){
	
		unsigned int threadExit = WaitForSingleObject(_threadExitEvent, 0);
		if(threadExit == WAIT_OBJECT_0){
			break;
		}
		
		wprintf(L"TPS: %d\n", tps);
		InterlockedExchange(&tps, 0);
		Sleep(999);
	}

	return 0;

}