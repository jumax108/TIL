#include <Windows.h>
#include <stdio.h>
#include <thread>
#include <list>
#include <Dbghelp.h>

#include "dump.h"
#include "UnsafeObjectFreeList.h"
#include "lockFreeQueue.h"

int size = 0;

struct stNode{

	int data = 0;

};
HANDLE _printTpsThread;


CLockFreeQueue<stNode*>* queue = new CLockFreeQueue<stNode*>();
CLockFreeQueue<stNode*>* _queueForDebug = nullptr;

CDump dump;

unsigned int tps = 0;

constexpr int THREAD_NUM = 5;
constexpr int NODE_EACH_THREAD = 5;
constexpr int MAX_NODE = THREAD_NUM * NODE_EACH_THREAD;

stNode _numArr[MAX_NODE];

HANDLE _logicThread[THREAD_NUM];

unsigned __stdcall logicTestFunc(void*);
unsigned __stdcall printTPS(void*);

int main(){

	_printTpsThread = (HANDLE)_beginthreadex(nullptr, 0, printTPS, nullptr, 0, nullptr);

	for(int nodeCnt = 0; nodeCnt < MAX_NODE; ++nodeCnt){

		queue->push(&_numArr[nodeCnt]);

	}

	for(int threadCnt = 0; threadCnt < THREAD_NUM; ++threadCnt){

		_logicThread[threadCnt] = (HANDLE)_beginthreadex(nullptr, 0, logicTestFunc, &_logicThread[threadCnt], 0, nullptr);

	}

	while(1);

	return 0;

}

unsigned __stdcall logicTestFunc(void* arg){

	///
	/// 
	/// 테스트 시나리오
	/// 
	/// 1. nodeNum 만큼의 노드를 큐에 push 한다.
	/// 2. 큐에서 nodeNum 만큼의 노드를 pop 한다.
	/// 3. 노드의 데이터가 초기 데이터인지 확인한다.					-> 다른 스레드에서 사용했는지 확인
	/// 4. 노드의 데이터를 1 증가한다.
	/// 5. 노드의 데이터가 1 인지 확인한다.								-> 다른 스레드에서 사용했는지 확인
	/// 6. 노드의 데이터를 1 감소한다.
	/// 7. 노드의 데이터가 0 인지 확인한다.								-> 다른 스레드에서 사용했는지 확인
	/// 8. 반복한다.
	/// 

	HANDLE threadHandle = *(HANDLE*)arg;
	int nodeNum = NODE_EACH_THREAD;
	stNode** arr = new stNode*[nodeNum];
	
	for(;;){


		for(int nodeCnt = 0; nodeCnt < nodeNum; ++nodeCnt){

			bool popResult = queue->pop(&arr[nodeCnt], threadHandle);
						
			stNode* node = arr[nodeCnt];

			if(popResult == false){
				// error
				_queueForDebug = queue;
				queue = nullptr;
				CDump::crash();
			}
		}

		for(int nodeCnt = 0; nodeCnt < nodeNum; ++nodeCnt){
			if(arr[nodeCnt]->data != 0){
				// error
				_queueForDebug = queue;
				queue = nullptr;
				CDump::crash();
			}
		}

		for(int nodeCnt = 0; nodeCnt < nodeNum; ++nodeCnt){
		
			stNode* node = arr[nodeCnt];
			InterlockedIncrement((unsigned int*)&node->data);
			
			
		}

		for(int nodeCnt = 0; nodeCnt < nodeNum; ++nodeCnt){
			if(arr[nodeCnt]->data != 1){
				// error
				_queueForDebug = queue;
				queue = nullptr;
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
				_queueForDebug = queue;
				queue = nullptr;
				CDump::crash();
			}
		}

		for(int nodeCnt = 0; nodeCnt < nodeNum; ++nodeCnt){


			stNode* node = arr[nodeCnt];

			bool pushResult = queue->push(node, threadHandle);
			
			if(pushResult == false){
				// error
				_queueForDebug = queue;
				queue = nullptr;
				CDump::crash();
			}
		}

		InterlockedIncrement(&tps);

	}

	return 0;
}

unsigned __stdcall printTPS(void*){

	while(1){
		Sleep(999);
		if(tps == 0){
			dump.crash();
		}
		wprintf(L"TPS: %d\n", tps);
		InterlockedExchange(&tps, 0);
	}

	return 0;

}