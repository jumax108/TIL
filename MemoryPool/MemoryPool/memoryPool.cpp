#include "memoryPool.h"

CMemoryPool::CMemoryPool(int allocatorNum){
	_allocatorArr = (stAllocator*)malloc(sizeof(stAllocator) * allocatorNum);
	_allocatorNum = 0;
}

void CMemoryPool::createAllocator(int memSize){
	
	int allocatorIndex = 0;
	stAllocator* allocator = _allocatorArr;

	// allocator ��ġ Ȯ��
	for(int allocatorCnt = 0; allocatorCnt < _allocatorNum ; ++allocatorCnt){
		if(_allocatorArr[allocatorCnt]._memSize < memSize){
			allocator = &_allocatorArr[allocatorCnt];
			allocatorIndex = allocatorCnt;
		}
	}

	// allocator �ڸ� Ȯ��
	for(int allocatorCnt = _allocatorNum - 1; allocatorCnt >= allocatorIndex ; --allocatorCnt){
		_allocatorArr[allocatorCnt + 1]._allocator	= _allocatorArr[allocatorCnt]._allocator;
		_allocatorArr[allocatorCnt + 1]._memSize	= _allocatorArr[allocatorCnt]._memSize;
	}

	// allocator ����
	allocator->_allocator = new CMemoryBlockList(memSize);
	allocator->_memSize = memSize;
}