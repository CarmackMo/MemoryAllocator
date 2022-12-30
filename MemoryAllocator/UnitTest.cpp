#include "MemoryAllocator.h"

#include <Windows.h>
#include <assert.h>
#include <algorithm>
#include <vector>

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _DEBUG


bool MemorySystem_UnitTest();
bool BitArray_UnitTest();
bool FixSizeAllocator_UnitTest();


int main(int i_arg, char**)
{

	/* BitArray Test */
	printf("Bit Array unit test begin \n");
	if (BitArray_UnitTest())
		printf("Bit Array unit test success! \n");



	/* Fix Allocator Test */
	printf("Fix Allocator unit test begin \n");
	if (FixSizeAllocator_UnitTest())
		printf("Fix Allocator unit test success! \n");



	/* Memory Allocator Test */
	const size_t 		sizeHeap = 1024 * 1024;
	const unsigned int 	numDescriptors = 2048;

	// Allocate memory for my test heap.
	void* pHeapMemory = HeapAlloc(GetProcessHeap(), 0, sizeHeap);
	assert(pHeapMemory);

	// Create your HeapManager and FixedSizeAllocators.
	InitializeMemoryAllocator(pHeapMemory, sizeHeap);

	printf("Memory system unit test begin \n");
	bool success = MemorySystem_UnitTest();
	if (success) { printf("Memory system unit test successful! \n"); }
	assert(success);

	// Clean up your Memory Allocator (DynamicAllocator and FixedSizeAllocators)
	DestroyMemoryAllocator();

	HeapFree(GetProcessHeap(), 0, pHeapMemory);

	// in a Debug build make sure we didn't leak any memory.
#if defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif // _DEBUG

	return 0;
}



bool MemorySystem_UnitTest()
{
	const size_t maxAllocations = 10 * 1024;
	std::vector<void*> AllocatedAddresses;

	long	numAllocs = 0;
	long	numFrees = 0;
	long	numCollects = 0;

	size_t totalAllocated = 0;

	// reserve space in AllocatedAddresses for the maximum number of allocation attempts
	// prevents new returning null when std::vector expands the underlying array
	AllocatedAddresses.reserve(10 * 1024);

	// allocate memory of random sizes up to 1024 bytes from the heap manager
	// until it runs out of memory
	do
	{
		const size_t maxTestAllocationSize = 1024;
		size_t sizeAlloc = 1 + (rand() & (maxTestAllocationSize - 1));
		void* pPtr = Alloc(sizeAlloc);

		// if allocation failed see if garbage collecting will create a large enough block
		if (pPtr == nullptr)
		{
			Collect();
			pPtr = Alloc(sizeAlloc);

			// if not we're done. go on to cleanup phase of test
			if (pPtr == nullptr)
				break;
		}

		AllocatedAddresses.push_back(pPtr);
		numAllocs++;

		totalAllocated += sizeAlloc;

		// randomly free and/or garbage collect during allocation phase
		const unsigned int freeAboutEvery = 0x07;
		const unsigned int garbageCollectAboutEvery = 0x07;

		if (!AllocatedAddresses.empty() && ((rand() % freeAboutEvery) == 0))
		{
			void* pPtrToFree = AllocatedAddresses.back();
			AllocatedAddresses.pop_back();

			Free(pPtrToFree);
			numFrees++;
		}
		else if ((rand() % garbageCollectAboutEvery) == 0)
		{
			Collect();
			numCollects++;
		}

	} while (numAllocs < maxAllocations);

	// now free those blocks in a random order
	if (!AllocatedAddresses.empty())
	{
		// randomize the addresses
		std::random_shuffle(AllocatedAddresses.begin(), AllocatedAddresses.end());

		// return them back to the heap manager
		while (!AllocatedAddresses.empty())
		{
			void* pPtrToFree = AllocatedAddresses.back();
			AllocatedAddresses.pop_back();

			delete[] pPtrToFree;
		}

		// do garbage collection
		Collect();
		// our heap should be one single block, all the memory it started with

		// do a large test allocation to see if garbage collection worked
		void* pPtr = Alloc(totalAllocated / 2);

		if (pPtr)
			Free(pPtr);
		else
			return false;
	}
	else
		return false;

	// this new [] / delete [] pair should run through your allocator
	char* pNewTest1 = new char[1024];
	delete[] pNewTest1;

	int* pNewTest2 = new int[1024];
	delete[] pNewTest2;

	long long* pNewTest3 = new long long;
	delete pNewTest3;

	// we succeeded
	return true;
}


bool BitArray_UnitTest()
{
	const size_t 		sizeHeap = 1024 * 1024;
	const unsigned int 	numDescriptors = 2048;

	void* pHeapMemory = HeapAlloc(GetProcessHeap(), 0, sizeHeap);
	assert(pHeapMemory);

	const size_t blockNum = 128;
	const size_t blockSize = 16;
	BitArray* bitArray = CreateBitArray(pHeapMemory, blockNum, true);

	printf("List address: %p \n", &bitArray->arr);

	*bitArray->FindElementPtr(0) = 0xFFFFFFFF;
	*bitArray->FindElementPtr(1) = 0x7FFFFFFF;
	*bitArray->FindElementPtr(2) = 0x0;

	printf("\nTest 1\n");
	printf("arr[%u]: %x\n", 0, *bitArray->FindElementPtr(0));
	printf("arr[%u]: %x\n", 1, *bitArray->FindElementPtr(1));
	printf("arr[%u]: %x\n", 2, *bitArray->FindElementPtr(2));
	size_t temp1, temp2;
	bool temp3;
	temp3 = bitArray->FindFirstAllocateBit(temp1);
	temp3 = bitArray->FindFirstFreeBit(temp2);
	assert(temp1 == 63);
	assert(temp2 == 0);

	printf("\nTest 2\n");
	assert(bitArray->IsBitSet(63) == false);
	assert(bitArray->IsBitSet(62) == true);
	assert(bitArray->IsBitClear(63) == true);
	assert(bitArray->IsBitClear(62) == false);
	assert(bitArray->AreAllBitsSet() == false);
	assert(bitArray->AreAllBitsClear() == false);
	bitArray->SetBit(63);
	printf("arr[%u]: %x\n", 0, *bitArray->FindElementPtr(0));
	printf("arr[%u]: %x\n", 1, *bitArray->FindElementPtr(1));
	printf("arr[%u]: %x\n", 2, *bitArray->FindElementPtr(2));
	assert(bitArray->IsBitSet(63) == true);
	assert(bitArray->IsBitSet(62) == true);
	assert(bitArray->IsBitClear(63) == false);
	assert(bitArray->IsBitClear(62) == false);
	bitArray->ClearBit(63);
	printf("arr[%u]: %x\n", 0, *bitArray->FindElementPtr(0));
	printf("arr[%u]: %x\n", 1, *bitArray->FindElementPtr(1));
	printf("arr[%u]: %x\n", 2, *bitArray->FindElementPtr(2));

	printf("\nTest 3\n");
	bitArray->SetAllBits();
	assert(bitArray->AreAllBitsSet() == true);
	assert(bitArray->AreAllBitsClear() == false);
	printf("arr[%u]: %x\n", 1, *bitArray->FindElementPtr(1));
	printf("arr[%u]: %x\n", 2, *bitArray->FindElementPtr(2));
	bitArray->ClearAllBits();
	assert(bitArray->AreAllBitsSet() == false);
	assert(bitArray->AreAllBitsClear() == true);
	printf("arr[%u]: %x\n", 1, *bitArray->FindElementPtr(1));
	printf("arr[%u]: %x\n", 2, *bitArray->FindElementPtr(2));

	return true;
}



bool FixSizeAllocator_UnitTest()
{
	const size_t 		sizeHeap = 1024 * 1024;
	const unsigned int 	numDescriptors = 2048;

	void* pHeapMemory = HeapAlloc(GetProcessHeap(), 0, sizeHeap);
	assert(pHeapMemory);

	const uint32_t blockNum = 128;
	const size_t blockSize = 16;
	FixSizeAllocator* allocator = CreateFixSizeAllocator(pHeapMemory, blockNum, blockSize, sizeHeap);

	printf("arr[%u]: %x\n", 0, *allocator->bitArray.FindElementPtr(0));
	void* ptr1 = allocator->Alloc();
	void* ptr2 = allocator->Alloc();
	void* ptr3 = allocator->Alloc();
	void* ptr4 = allocator->Alloc();
	printf("arr[%u]: %x\n", 0, *allocator->bitArray.FindElementPtr(0));
	assert(*allocator->bitArray.FindElementPtr(0) == 0xFFFFFFF0);

	allocator->Free(ptr3);
	printf("arr[%u]: %x\n", 0, *allocator->bitArray.FindElementPtr(0));
	assert(*allocator->bitArray.FindElementPtr(0) == 0xFFFFFFF4);
	allocator->Free(ptr2);
	printf("arr[%u]: %x\n", 0, *allocator->bitArray.FindElementPtr(0));
	assert(*allocator->bitArray.FindElementPtr(0) == 0xFFFFFFF6);
	allocator->Free(ptr4);
	printf("arr[%u]: %x\n", 0, *allocator->bitArray.FindElementPtr(0));
	assert(*allocator->bitArray.FindElementPtr(0) == 0xFFFFFFFE);
	allocator->Free(ptr1);
	printf("arr[%u]: %x\n", 0, *allocator->bitArray.FindElementPtr(0));
	assert(*allocator->bitArray.FindElementPtr(0) == 0xFFFFFFFF);

	return true;
}