#pragma once
#include "BirArray.h"
#include "../Utility/Utility.h"


using namespace Utility;


/**
* @brief FixSizeAllocator is a memory allocator that designed for small-size memory allocation. The 
*		 size of each memory blocks in fix size allocator is defined by user and fixed during runtime.
*		 Fix size allocator uses bit array to manage its memory blocks. A fix size allocator is make up
*		 of two parts. The fist part is the memory space for storing datas (member variable, etc.) and 
*		 the second part is the memory space for memory allocations. The structure of fix size allocator 
*		 be like:
*		 |  member variables  |  bit array  |  memory blocks... | ... | ... | ... | ...... |
* 
* @param blockNum -- The total number of memory blocks in fix size allocator;
* @param freeBlockNum -- The number of free memory blocks at current stage;
* @param blockSize - The size of memory block;
* @param bitArraySize -- The total size of bit array;
* @param blockBaseAddr -- A variable that stores the starting address of the first memory block;
* @param bitArray -- BitArray instance. Note that "bitArray" must be the last member otherwise the
*		 next member will have memory overlap with "bitArray". See the description of "BitArray"
*		 class for more detail;
*/
class FixSizeAllocator
{
public:
	size_t blockNum;
	size_t freeBlockNum;
	size_t blockSize;
	size_t bitArraySize;
	void* blockBaseAddr;
	BitArray bitArray;


	inline FixSizeAllocator(
		BitArray bitArray, 
		size_t blockNum = 0, size_t freeBlockNum = 0, 
		size_t blockSize = 0, size_t bitArraySize = 0,
		void* blockBaseAddr = nullptr);
	inline ~FixSizeAllocator();


	/**
	* @brief Detect whether the given memory address is existed in current fix allocator
	*		 Case 1: given memory address does not match to memory block offset.
	*		 Case 2: given memory address does not within the range of the bit array.
	*/
	bool Contains(const void* ptr) const;

	/**
	* @brief Detect whether the provided memory address is already allocated. Noted that the method
	*		 will return false if the memory address does not existed in fix allocator
	* 
	* @return If the given memory block does not exist in fix size allocator, return false as well.
	*/
	bool IsAllocated(const void* ptr) const;

	/**
	* @brief Need to check whether the given memory address is valid and whether the given memory address
	*		 is already allocated before release the memory
	* 
	* @return If given memory block does not exist in fix size allocator or the given memory 
	*		  block does not allocated, return false. Otherwise, release is success, return true.
	*/
	bool Free(void* ptr);

	/**
	* @brief Find first free block. Mark the block as "Allocated". Return the block address
	*/
	void* Alloc();

	void Destroy();
};


/**
* @brief Instantiate a FixSizeAllocator instance in the designated memory space. It is done by
*		 manually assigning a block of unoccupied memory space to store the FixSizeAllocator
*		 instance and telling the compiler to treat that memory space as a FixSizeAllocator.
*/
FixSizeAllocator* CreateFixSizeAllocator(void* baseAddr, size_t blockNum, size_t blockSize, size_t heapSize);

#include "FixSizeAllocator.inl"

