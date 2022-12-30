#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../Utility/Utility.h"


using namespace Utility;


/**
* @brief MemoryBlock is a linked list node that act as the minimum memory management unit of 
*		 dynamic allocator. A MemoryBlock node is make up of two parts. The first part is the 
*		 memory space that containing the data of memory block and the second part is the 
*		 memory space that dynamic allocator allocates to user. Assuming user request 10 byte 
*		 of memory on address 0x0000. Dyanimc allocator first needs to assign 32B of memory to 
*		 construct a "MemoryBlock" and then assign 10B of memory to the "MemoryBlock" for user.
*		 Therefore, we need 32B + 10B in total and the structure looks like:
*			|     memory block data: 32B     |      user memory: 10B         |
*		 0x0000								0x0020						  0x002A
*
* @param baseAddr -- The starting address of the memory space that assigned to user;
* @param blockSize -- The size of the memory space that assigned to user;
* @param nextBlock -- Address of the next "MemoryBlock" node;
* @param prevBlock -- Address of the previous "MemoryBlock" node;
*/
class MemoryBlock
{
public:
	void* baseAddr;
	size_t blockSize;
	MemoryBlock* nextBlock;
	MemoryBlock* prevBlock;

	inline MemoryBlock(void* addr, size_t size, MemoryBlock* next, MemoryBlock* prev);
	inline ~MemoryBlock();
};


/**
* @brief DyanmicAllocator is a memory allocator that designed for general memory allocation.
*		 DynamicAllocator use two linked lists to manage its memory. One linked list for 
*		 managing free menory blocks in allocator and the other for managing allocated menory
*		 blocks. (See the description of "MemoryBlock" class for more detail)
*
* @param baseAddr -- The starting address of the dynamic allocator, which is also the starting
*					 address of the whole memory space;
* @param heapSize -- The size of the whole memory space, including the memory space for storing
*					 dynamic allocator data and memory space for allocation;
* @param freeList -- The address of the linked list that manage free memory blocks;
* @param allocList -- The address of the linked list that manage allocated memory blocks;
*/
class DynamicAllocator
{
public:
	/* Member Field */
	void* baseAddr;
	size_t heapSize;
	MemoryBlock* freeList;
	MemoryBlock* allocList;

	/* Method Field */
	inline DynamicAllocator(void* addr, size_t size);
	inline ~DynamicAllocator();

	MemoryBlock* ShrinkMemoryBlock(MemoryBlock* block, size_t shrinkSize);

	inline void* Alloc(size_t size);

	void* Alloc(size_t size, const unsigned int alignment);

	bool Free(void* ptr);

	void Collect();

	void Destroy();

	void AddAllocBlockToList(MemoryBlock* allocBlock);
	void AddFreeBlockToList(MemoryBlock* freeBlock);

	bool Contains(const void* ptr) const;
	bool IsAllocated(const void* ptr) const;

	size_t GetLargestFreeBlock() const;
	size_t GetTotalFreeMemory() const;

	void ShowFreeBlocks() const;
	void ShowOutstandingAllocations()const;
};


const size_t MANAGER_SIZE = sizeof(DynamicAllocator);
const size_t BLOCK_SIZE = sizeof(MemoryBlock);


/* Function Space */
DynamicAllocator* CreateDynamicAllocator(void* baseAddr, size_t size);

MemoryBlock* CreateMemoryBlock(void* ptr, size_t size);

#include "DynamicAllocator.inl"
