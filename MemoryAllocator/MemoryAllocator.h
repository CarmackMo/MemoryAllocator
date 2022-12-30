#pragma once
#include <assert.h>
#include "DynamicAllocator/DynamicAllocator.h"
#include "FixSizeAllocator/FixSizeAllocator.h"


struct FixSizeAllocatorArg
{
	size_t blockSize;
	size_t blockNum;
};
extern const int fixSizeAllocatorNum;
extern const FixSizeAllocatorArg fixSizeAllocatorDatas[];
extern FixSizeAllocator* fixSizeAllocatorPtrs[];
extern DynamicAllocator* dynamicAllocator;



// InitializeMemoryAllocator - initialize your memory system including your HeapManager and some FixedSizeAllocators
bool InitializeMemoryAllocator(void * i_pHeapMemory, size_t i_sizeHeapMemory);

// DestroyMemoryAllocator - destroy your memory systems
void DestroyMemoryAllocator();

void* Alloc(size_t size);

void Free(void* ptr);

// Collect - coalesce free blocks in attempt to create larger blocks
void Collect();

void* operator new(size_t size);

void* operator new[](size_t size);

void operator delete(void* ptr);

void operator delete[](void* ptr);
