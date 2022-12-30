#pragma once


inline FixSizeAllocator::FixSizeAllocator(
	BitArray bitArray,
	size_t blockNum, size_t freeBlockNum,
	size_t blockSize, size_t bitArraySize,
	void* blockBaseAddr)
{
	this->blockNum = blockNum;
	this->freeBlockNum = freeBlockNum;
	this->blockSize = blockSize;
	this->bitArraySize = bitArraySize;
	this->blockBaseAddr = blockBaseAddr;
	this->bitArray = bitArray;
}


inline FixSizeAllocator::~FixSizeAllocator() {}