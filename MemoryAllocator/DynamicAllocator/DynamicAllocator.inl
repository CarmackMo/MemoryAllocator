#pragma once


inline MemoryBlock::MemoryBlock(void* addr, size_t size, MemoryBlock* next, MemoryBlock* prev)
{
	this->baseAddr = addr;
	this->blockSize = size;
	this->nextBlock = next;
	this->prevBlock = prev;
}


inline MemoryBlock::~MemoryBlock() {}


inline DynamicAllocator::DynamicAllocator(void* addr, size_t size)
{
	this->baseAddr = addr;
	this->heapSize = size;
	this->allocList = nullptr;
	this->freeList = nullptr;
}


inline DynamicAllocator::~DynamicAllocator() {}


inline void* DynamicAllocator::Alloc(size_t size)
{
	return this->Alloc(size, 0);
}

