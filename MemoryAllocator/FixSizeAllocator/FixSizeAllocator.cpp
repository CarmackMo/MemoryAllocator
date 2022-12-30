#include "FixSizeAllocator.h"


FixSizeAllocator* CreateFixSizeAllocator(void* baseAddr, size_t blockNum, size_t blockSize, size_t heapSize)
{
	FixSizeAllocator* allocator = static_cast<FixSizeAllocator*>(baseAddr);
	allocator->blockNum = blockNum;
	allocator->freeBlockNum = blockNum;
	allocator->blockSize = blockSize;
	allocator->bitArray = *CreateBitArray(&allocator->bitArray, blockNum, true);
	BitArray bitArray = allocator->bitArray;
	allocator->bitArraySize = reinterpret_cast<uintptr_t>(PointerSub(PointerAdd(&bitArray.arr, bitArray.length * sizeof(BitElement)), &bitArray));
	allocator->blockBaseAddr = PointerAdd(&allocator->bitArray, allocator->bitArraySize);

	/* Debug */
	//size_t temp1 = sizeof(size_t) * 2 + sizeof(BitElement) * fixAllocator->bitArray.length;
	//size_t temp2 = reinterpret_cast<uintptr_t>(PointerSub(PointerAdd(&bitArray.arr, bitArray.length * sizeof(BitElement)), &bitArray));
	//size_t temp3 = sizeof(size_t);
	//printf("BlockNum address %p \n", &fixAllocator->blockNum);
	//printf("BlockSize address %p \n", &fixAllocator->blockSize);
	//printf("Size address: %p \n", &fixAllocator->bitArraySize);
	//printf("Memory address: %p \n", &fixAllocator->blockBaseAddr);
	//printf("BitArray address %p \n", &fixAllocator->bitArray);
	/**/

	size_t allocatorSize = reinterpret_cast<uintptr_t>(PointerSub(PointerAdd(allocator->blockBaseAddr, blockSize * blockNum), baseAddr));
	if (allocatorSize > heapSize)
		return nullptr;

	return allocator;
}


bool FixSizeAllocator::Contains(const void* ptr) const
{
	uintptr_t offset = reinterpret_cast<uintptr_t>(PointerSub(ptr, this->blockBaseAddr));
	size_t blockIdx = offset / this->blockSize;

	if (offset % this->blockSize != 0)
		return false;
	else if (blockIdx > this->blockNum)
		return false;
	else
		return true;
}


bool FixSizeAllocator::IsAllocated(const void* ptr) const
{
	uintptr_t offset = reinterpret_cast<uintptr_t>(PointerSub(ptr, this->blockBaseAddr));
	size_t blockIdx = offset / this->blockSize;

	if (this->Contains(ptr))
		return this->bitArray.IsBitClear(blockIdx);
	else
		return false;
}


bool FixSizeAllocator::Free(void* ptr)
{
	uintptr_t offset = reinterpret_cast<uintptr_t>(PointerSub(ptr, this->blockBaseAddr));
	size_t blockIdx = offset / this->blockSize;

	if (this->freeBlockNum == this->blockNum)
		return false;
	else if (!this->Contains(ptr))
	{
		/* Debug */
		//printf("WARNING: FixAllocator.Free(): Given memory address is invalid! %p \n", ptr);
		/**/
		return false;
	}
	else if (!this->IsAllocated(ptr))
	{
		/* Debug */
		//printf("WARNING: FixAllocator.Free(): Trying to double free the given memory address! %p \n", ptr);
		/**/
		return false;
	}
	else
	{
		this->freeBlockNum++;
		this->bitArray.SetBit(blockIdx);
		return true;
	}
}


void* FixSizeAllocator::Alloc()
{
	if (this->freeBlockNum == 0)
		return nullptr;

	size_t bitIdx;
	bool success = this->bitArray.FindFirstFreeBit(bitIdx);

	if (!success)
		return nullptr;
	else
	{
		this->freeBlockNum--;
		this->bitArray.ClearBit(bitIdx);
		return PointerAdd(this->blockBaseAddr, blockSize * bitIdx);
	}
}


void FixSizeAllocator::Destroy()
{
	if (!this->bitArray.AreAllBitsSet())
	{
		size_t bitIdx;
		this->bitArray.FindFirstFreeBit(bitIdx);
		void* ptr = PointerAdd(this->blockBaseAddr, sizeof(BitElement) * bitIdx);
		printf("WARNING: FixAllocator.~FixAllocator(): Detect memory leak at %p \n", ptr);
	}
}
