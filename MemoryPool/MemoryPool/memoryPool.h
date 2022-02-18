#pragma once

#define MEMORY_POOL_SAFE

#include "memoryBlockList.h"

class CMemoryPool{

	struct stAllocator;

public:

	CMemoryPool(int allocatorNum);

	void	createAllocator(int memSize);

	void*	allocMem	(int memSize);
	void	freeMem		(void* ptr);

private:
	// allocator num
	int _allocatorNum;
	stAllocator* _allocatorArr;

	struct stAllocator{
		int _memSize;
		CMemoryBlockList* _allocator;
	};

};
