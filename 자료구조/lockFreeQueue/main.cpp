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
	/// �׽�Ʈ �ó�����
	/// 
	/// 1. nodeNum ��ŭ�� ��带 ť�� push �Ѵ�.
	/// 2. ť���� nodeNum ��ŭ�� ��带 pop �Ѵ�.
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