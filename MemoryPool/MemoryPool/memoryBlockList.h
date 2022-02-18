#pragma once

#include <malloc.h>

#if defined(MEMORY_POOL_SAFE)

	/* block 구조
	* memory size 4byte
	* underflow check 8byte (safe)
	* actual memory (가변)
	* overflow check 8byte (safe)
	* duplicate free check 1byte (safe)
	*/

	#include "errorCode.h"
	constexpr unsigned __int64	UNDERFLOW_CHECK_VALUE	= 0xFFEEDDCCCCDDEEFF;
	constexpr unsigned __int64	OVERFLOW_CHECK_VALUE	= 0xFFEEDDCCCCDDEEFF;

	#define mallocSize(memSize) (memSize + 21)
	#define memoryToBlock(mem) ((void*)((unsigned __int64)mem - 12))
	#define blockToMemorySize(block) ((void*)((unsigned __int64)block))
	#define blockToMemory(block) ((void*)((unsigned __int64)block + 12))
	#define blockToUnderflow(block) ((unsigned __int64)block)
	#define blockToOverflow(block, memSize) ((unsigned __int64)block + memSize + 12)
	#define blockToDuplicateFree(block, memSize) ((bool*)block + memSize + 20)

#else
	#define mallocSize(memSize) ((void*)((unsigned __int64)mem - 4))
	#define blockToMemory(block) ((void*)((unsigned __int64)block + 4))
#endif

class CMemoryBlockList {

	struct stBlockList;
	friend class CMemoryPool;

private:

	CMemoryBlockList(int memSize);
	~CMemoryBlockList();

	void* allocMem();
	bool freeMem(void*);

	// 할당 크기
	int _memSize;

	// 할당 가능한 블럭 리스트
	stBlockList* _allocAbleList;

	#ifdef MEMORY_POOL_SAFE
		// 에러 코드 얻는 용도
		int lastError = 0;
		inline int getLastError();
	#endif

	struct stBlockList {
		void* _block = nullptr;
		stBlockList* _next = nullptr;
	};


};


CMemoryBlockList::CMemoryBlockList(int memSize){
	_memSize = memSize;
	_allocAbleList = nullptr;
}

CMemoryBlockList::~CMemoryBlockList(){

	while(_allocAbleList != nullptr){
		stBlockList* next = _allocAbleList->_next;
		free(_allocAbleList->_block);
		free(_allocAbleList);
		_allocAbleList = next;
	}
}

void* CMemoryBlockList::allocMem(){

	if(_allocAbleList == nullptr){
		_allocAbleList = (stBlockList*)malloc(sizeof(stBlockList));
		_allocAbleList->_block = malloc(mallocSize(_memSize));
	}

	void* resultPtr = blockToMemory(_allocAbleList->_block);
	_allocAbleList = _allocAbleList->_next;
	
	#ifdef MEMORY_POOL_SAFE
		*blockToDuplicateFree(_allocAbleList, _memSize) = true;
	#endif

	return resultPtr;
}

bool CMemoryBlockList::freeMem(void* memPtr){
	
	stBlockList* usedBlockList;
	#if defined(MEMORY_POOL_SAFE)
	{
		void* usedBlock = memoryToBlock(memPtr);
		if(*blockToDuplicateFree(usedBlock, _memSize) == false){
			lastError = DuplicatedFree;
			return false;
		}
		if(blockToUnderflow(usedBlock) != UNDERFLOW_CHECK_VALUE){
			lastError = UnderFlow;
			return false;
		}
		if(blockToOverflow(usedBlock) != OVERFLOW_CHECK_VALUE){
			lastError = OverFlow;
			return false;
		}
		*blockToDuplicateFree(usedBlock, _memSize) = false;
		usedBlockList = (stBlockList*)usedBlock;
	}
	#else
		usedBlock = (stBlockList*)memPtr;
	#endif

	usedBlockList->_next = _allocAbleList;
	_allocAbleList = usedBlockList;

	return true;

}

int CMemoryBlockList::getLastError(){
	return lastError;
}