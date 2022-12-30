#include "MemoryAllocator.h"
#include "Utility/Utility.h"

using namespace Utility;

const int fixSizeAllocatorNum = 3;
const FixSizeAllocatorArg fixSizeAllocatorDatas[] = {
	{16, 100},
	{32, 200},
	{96, 400},
};
FixSizeAllocator* fixSizeAllocatorPtrs[fixSizeAllocatorNum] = { nullptr };
DynamicAllocator* dynamicAllocator;




bool InitializeMemoryAllocator(void* i_pHeapMemory, size_t i_sizeHeapMemory)
{
	void* baseAddr = i_pHeapMemory;
	for (int i = 0; i < fixSizeAllocatorNum; i++)
	{
		FixSizeAllocatorArg arg = fixSizeAllocatorDatas[i];
		FixSizeAllocator* allocator = CreateFixSizeAllocator(baseAddr, arg.blockNum, arg.blockSize, i_sizeHeapMemory);
		if (allocator != nullptr)
		{
			baseAddr = PointerAdd(allocator->blockBaseAddr, allocator->blockSize * allocator->blockNum);
			fixSizeAllocatorPtrs[i] = allocator;
		}
	}

	size_t fixAllocatorSize = reinterpret_cast<uintptr_t>(PointerSub(baseAddr, i_pHeapMemory));
	dynamicAllocator = CreateDynamicAllocator(baseAddr, i_sizeHeapMemory - fixAllocatorSize);

	/* Debug */
	//assert(reinterpret_cast<uintptr_t>(FixAllocatorPtrs[1]) == reinterpret_cast<uintptr_t>(PointerAdd(i_pHeapMemory, 1672)));
	//assert(reinterpret_cast<uintptr_t>(FixAllocatorPtrs[2]) == reinterpret_cast<uintptr_t>(PointerAdd(i_pHeapMemory, 1672 + 6484)));
	//assert(reinterpret_cast<uintptr_t>(heapAllocator) == reinterpret_cast<uintptr_t>(PointerAdd(i_pHeapMemory, 1672 + 6484 + 38508)));
	/**/


	for (int i = 0; i < fixSizeAllocatorNum; i++)
	{
		if (fixSizeAllocatorPtrs[i] != nullptr)
			return true;
	}

	return dynamicAllocator != nullptr ? true : false;
}


void Collect()
{
	dynamicAllocator->Collect();
}


void DestroyMemoryAllocator()
{
	for (int i = 0; i < fixSizeAllocatorNum; i++)
	{
		fixSizeAllocatorPtrs[i]->Destroy();
	}
	dynamicAllocator->Destroy();
}


void* Alloc(size_t size)
{
	void* ptr = nullptr;
	for (int i = 0; i < fixSizeAllocatorNum; i++)
	{
		if (size <= fixSizeAllocatorDatas[i].blockSize && fixSizeAllocatorPtrs[i] != nullptr)
		{
			ptr = fixSizeAllocatorPtrs[i]->Alloc();

			if (ptr != nullptr)
				return ptr;
		}
	}

	/* At this point, all fix allocator allocation attempts are fail. Otherwise, the function
	 * is already returned. Heap allocator is the last attempt to allocate memory for the user */
	if (dynamicAllocator != nullptr)
		return dynamicAllocator->Alloc(size);
	else
		return nullptr;
}


void Free(void* ptr)
{
	bool success = false;
	for (int i = 0; i < fixSizeAllocatorNum; i++)
	{
		if (!success && fixSizeAllocatorPtrs[i] != nullptr)
			success = fixSizeAllocatorPtrs[i]->Free(ptr);

		if (success)
			return;
	}

	/* At this point, all fix allocator free attempts are fail. Otherwise, the function
	 * is already returned. Heap allocator is the last attempt to free the memory. */
	if (dynamicAllocator != nullptr)
		success = dynamicAllocator->Free(ptr);
	if (!success)
		printf("Allocators.free(): Unable to free the given memory address. %p \n", ptr);
}


void* operator new(size_t size)
{
	return Alloc(size);
}


void* operator new[](size_t size)
{
	return Alloc(size);
}


void operator delete(void* ptr)
{
	return Free(ptr);
}

void operator delete[](void* ptr)
{
	return Free(ptr);
}