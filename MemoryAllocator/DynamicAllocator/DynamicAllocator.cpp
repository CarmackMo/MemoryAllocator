#include "DynamicAllocator.h"



DynamicAllocator* CreateDynamicAllocator(void* baseAddr, size_t size)
{
	DynamicAllocator* allocator = static_cast<DynamicAllocator*>(baseAddr);
	allocator->baseAddr = baseAddr;
	allocator->heapSize = size;
	allocator->freeList = nullptr;
	allocator->allocList = nullptr;

	/* The size of the heap memory block should be larger than the size
		of heap allocator plus the size of the first freeBlock node. The rest
		memory space the actual free memory*/
	if (size - MANAGER_SIZE - BLOCK_SIZE < 0)
		return nullptr;

	void* firstBlockAddr = PointerAdd(baseAddr, MANAGER_SIZE);
	MemoryBlock* freeBlock = CreateMemoryBlock(firstBlockAddr, size - MANAGER_SIZE - BLOCK_SIZE);
	allocator->freeList = freeBlock;

	return allocator;
}


MemoryBlock* CreateMemoryBlock(void* ptr, size_t size)
{
	MemoryBlock* block = static_cast<MemoryBlock*>(ptr);
	void* baseAddr = PointerAdd(ptr, BLOCK_SIZE);;
	block->baseAddr = baseAddr;
	block->blockSize = size;
	block->nextBlock = nullptr;
	block->prevBlock = nullptr;

	return block;
}


MemoryBlock* DynamicAllocator::ShrinkMemoryBlock(MemoryBlock* block, size_t shrinkSize)
{
	MemoryBlock* newBlock;
	newBlock = static_cast<MemoryBlock*>(PointerAdd(block, shrinkSize));
	newBlock->baseAddr = PointerAdd(block->baseAddr, shrinkSize);
	newBlock->blockSize = block->blockSize - shrinkSize;
	newBlock->nextBlock = block->nextBlock;
	newBlock->prevBlock = block->prevBlock;

	if (block->prevBlock != nullptr)
		block->prevBlock->nextBlock = newBlock;
	else
	{
		if (this->freeList == block)
			this->freeList = newBlock;
		else
			this->allocList = newBlock;
	}

	if (block->nextBlock != nullptr)
		block->nextBlock->prevBlock = newBlock;

	return newBlock;
}


void* DynamicAllocator::Alloc(size_t size, const unsigned int alignment)
{
	/* Align the size */
	if (alignment != 0 && size % alignment != 0)
		size = alignment * ((size + alignment) / alignment);

	/* Try to find free block that has sufficient size */
	MemoryBlock* freeBlock = this->freeList;
	while (freeBlock != nullptr)
	{
		if (freeBlock->blockSize >= size + BLOCK_SIZE)
		{
			/* If free block is not starting with an aligned address, create buffer block to make it align  */
			if (alignment != 0 && reinterpret_cast<uintptr_t>(PointerSub(freeBlock->baseAddr, this->baseAddr)) % alignment != 0)
			{
				/* (base address) + (node size of buffer) + (size of buffer) should satisfy the alignment requirement */
				size_t bufferSize = alignment - ((reinterpret_cast<uintptr_t>(freeBlock->baseAddr) + BLOCK_SIZE) % alignment);

				if (bufferSize == 0)
					bufferSize += alignment;

				if (freeBlock->blockSize >= size + (BLOCK_SIZE + bufferSize) + BLOCK_SIZE)
				{
					MemoryBlock* newAddr = this->ShrinkMemoryBlock(freeBlock, bufferSize + BLOCK_SIZE);
					MemoryBlock* bufferBlock = CreateMemoryBlock(freeBlock, bufferSize);
					this->AddFreeBlockToList(bufferBlock);
					freeBlock = newAddr;

					break;
				}
			}
			else
				break;
		}

		freeBlock = freeBlock->nextBlock;
	}

	/* Return NULL pointer if there is no free memory to allocate */
	if (freeBlock == nullptr)
		return nullptr;


	/* Allocate the memory to the block */
	this->ShrinkMemoryBlock(freeBlock, size + BLOCK_SIZE);
	MemoryBlock* allocBlock = CreateMemoryBlock(freeBlock, size);
	this->AddAllocBlockToList(allocBlock);

	return allocBlock->baseAddr;
}


bool DynamicAllocator::Free(void* ptr)
{
	MemoryBlock* block = this->allocList;
	while (block != nullptr)
	{
		if (block->baseAddr == ptr)
		{
			MemoryBlock* prev = block->prevBlock;
			MemoryBlock* next = block->nextBlock;

			if (prev == nullptr)
				this->allocList = next;
			else
				prev->nextBlock = next;

			if (next != nullptr)
				next->prevBlock = prev;

			this->AddFreeBlockToList(block);
			return true;
		}
		else
			block = block->nextBlock;
	}

	return false;
}


void DynamicAllocator::Collect()
{
	MemoryBlock* block = this->freeList;

	while (block != nullptr && block->nextBlock != nullptr)
	{
		if (reinterpret_cast<uintptr_t>(block->baseAddr) + block->blockSize == reinterpret_cast<uintptr_t>(block->nextBlock))
		{
			block->blockSize += BLOCK_SIZE + block->nextBlock->blockSize;

			MemoryBlock* nextNextBlock = block->nextBlock->nextBlock;
			if (nextNextBlock != nullptr)
				nextNextBlock->prevBlock = block;

			block->nextBlock = nextNextBlock;
		}
		else
			block = block->nextBlock;
	}
}


void DynamicAllocator::Destroy()
{
	MemoryBlock* block = this->freeList;
	while (block != nullptr)
	{
		MemoryBlock* tempBlock = block;
		block = block->nextBlock;
		tempBlock->nextBlock = nullptr;
		tempBlock->prevBlock = nullptr;
		tempBlock = nullptr;
		delete tempBlock;
	}

	block = this->allocList;
	while (block != nullptr)
	{
		MemoryBlock* tempBlock = block;
		block = block->nextBlock;
		tempBlock->nextBlock = nullptr;
		tempBlock->prevBlock = nullptr;
		tempBlock = nullptr;
		delete tempBlock;
	}

	//allocator = nullptr;
}


void DynamicAllocator::AddAllocBlockToList(MemoryBlock* allocBlock)
{
	if (this->allocList == nullptr)
	{
		allocBlock->prevBlock = nullptr;
		allocBlock->nextBlock = nullptr;
	}
	else
	{
		allocBlock->prevBlock = nullptr;
		allocBlock->nextBlock = this->allocList;
		this->allocList->prevBlock = allocBlock;
	}

	this->allocList = allocBlock;
}


void DynamicAllocator::AddFreeBlockToList(MemoryBlock* freeBlock)
{
	MemoryBlock* prevBlock = this->freeList;
	MemoryBlock* currBlock = this->freeList;

	/* If the free list is empty */
	if (currBlock == nullptr)
	{
		freeBlock->nextBlock = nullptr;
		freeBlock->prevBlock = nullptr;
		this->freeList = freeBlock;
		return;
	}

	/* If the new free block have the most forward address */
	if (freeBlock->baseAddr < currBlock->baseAddr)
	{
		freeBlock->nextBlock = currBlock;
		freeBlock->prevBlock = nullptr;
		currBlock->prevBlock = freeBlock;
		this->freeList = freeBlock;
		return;
	}

	/* If the new free block have a middle address */
	currBlock = currBlock->nextBlock;
	while (currBlock != nullptr)
	{
		if (freeBlock->baseAddr < currBlock->baseAddr)
		{
			freeBlock->prevBlock = prevBlock;
			freeBlock->nextBlock = currBlock;
			prevBlock->nextBlock = freeBlock;
			currBlock->prevBlock = freeBlock;
			return;
		}
		else
		{
			currBlock = currBlock->nextBlock;
			prevBlock = prevBlock->nextBlock;
		}
	}

	/* If the new free block have the last address */
	freeBlock->prevBlock = prevBlock;
	freeBlock->nextBlock = nullptr;
	prevBlock->nextBlock = freeBlock;

	return;
}


bool DynamicAllocator::Contains(const void* ptr) const
{
	MemoryBlock* block = this->allocList;
	while (block != nullptr)
	{
		if (block->baseAddr == ptr)
			return true;
		else
			block = block->nextBlock;
	}

	block = this->freeList;
	while (block != nullptr)
	{
		if (block->baseAddr == ptr)
			return true;
		else
			block = block->nextBlock;
	}

	return false;
}


bool DynamicAllocator::IsAllocated(const void* ptr) const
{
	MemoryBlock* block = this->allocList;
	while (block != nullptr)
	{
		if (block->baseAddr == ptr)
			return true;
		else
			block = block->nextBlock;
	}
	return false;
}


size_t DynamicAllocator::GetLargestFreeBlock() const
{
	size_t result = 0;
	MemoryBlock* freeBlock = this->freeList;

	while (freeBlock != nullptr)
	{
		if (freeBlock->blockSize > result)
			result = freeBlock->blockSize;

		freeBlock = freeBlock->nextBlock;
	}

	return result;
}


size_t DynamicAllocator::GetTotalFreeMemory() const
{
	size_t result = 0;
	MemoryBlock* freeBlock = this->freeList;

	while (freeBlock != nullptr)
	{
		result += freeBlock->blockSize;
		freeBlock = freeBlock->nextBlock;
	}

	return result;
}


void DynamicAllocator::ShowFreeBlocks() const
{
	printf("\n\n!!!###########################!!! \n Start showing free blocks: ");

	MemoryBlock* block = this->freeList;
	while (block != nullptr)
	{
		printf("\n--------------------------------------------------------------\n");
		printf("Free Block Node Address: %p, Free Block Address: %p, Free Block Size: %zu",
			block, block->baseAddr, block->blockSize);
		block = block->nextBlock;
	}
}


void DynamicAllocator::ShowOutstandingAllocations() const
{
	printf("\n\n!!!###########################!!! \n Start showing allocated blocks: ");

	MemoryBlock* block = this->allocList;
	while (block != nullptr)
	{
		printf("\n--------------------------------------------------------------\n");
		printf("Alloc Block Node Address: %p, Alloc Block Address: %p, Alloc Block Size: %zu",
			block, block->baseAddr, block->blockSize);
		block = block->nextBlock;
	}
}
