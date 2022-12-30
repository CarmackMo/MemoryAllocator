#include "BirArray.h"


BitArray* CreateBitArray(void* baseAddr, size_t blockNum, bool initToOne)
{
	BitArray* bitArray = static_cast<BitArray*>(baseAddr);
	bitArray->blockPerElement = sizeof(BitElement) * 8;
	bitArray->length = blockNum / bitArray->blockPerElement + (blockNum % bitArray->blockPerElement == 0 ? 0 : 1);
	bitArray->SetAllBits();

	return bitArray;
}


bool BitArray::FindFirstAllocateBit(size_t& outIdx) const
{
	unsigned long idx, bitIdx;
	BitElement marker = _WIN32 ? ~0UL : ~0ULL;			// Create an element that has 1 in all its bits
	BitElement element = 0;;
	for (idx = 0; idx < this->length; idx++)			// Skip scanning the element if all its bits are 1
	{
		element = *this->FindElementPtr(idx);
		if (element != marker)
			break;
	}

#if _WIN32
	_BitScanForward(&bitIdx, ~element);
#else
	_BitScanForward64(&bitIdx, ~element);
#endif

	if (idx >= this->length)
		return false;
	else
	{
		outIdx = idx * this->blockPerElement + bitIdx;
		return true;
	}
}


bool BitArray::FindFirstFreeBit(size_t& outIdx) const
{
	unsigned long idx, bitIdx;
	BitElement marker = _WIN32 ? 0UL : 0ULL;			// Create an element that has 0 in all its bits
	BitElement element = 0;
	for (idx = 0; idx < this->length; idx++)			// Skip scanning the element if all its bits are 0
	{
		element = *this->FindElementPtr(idx);
		if (element != marker)
			break;
	}

#if _WIN32
	_BitScanForward(&bitIdx, element);
#else
	_BitScanForward64(&bitIdx, element);
#endif

	if (idx >= this->length)
		return false;
	else
	{
		outIdx = idx * this->blockPerElement + bitIdx;
		return true;
	}
}


void BitArray::SetBit(size_t blockIdx)
{
	size_t idx = blockIdx / this->blockPerElement;
	size_t bitIdx = blockIdx % this->blockPerElement;

	if (_WIN32)
		*this->FindElementPtr(idx) |= 1UL << bitIdx;
	else
		*this->FindElementPtr(idx) |= 1ULL << bitIdx;
}


void BitArray::ClearBit(size_t blockIdx)
{
	size_t idx = blockIdx / this->blockPerElement;
	size_t bitIdx = blockIdx % this->blockPerElement;

	if (_WIN32)
		*this->FindElementPtr(idx) &= ~(1UL << bitIdx);
	else
		*this->FindElementPtr(idx) &= ~(1ULL << bitIdx);
}


void BitArray::SetAllBits()
{
	for (size_t i = 0; i < this->length; i++)
	{
		*this->FindElementPtr(i) = _WIN32 ? ~0UL : ~0ULL;
	}
}


void BitArray::ClearAllBits()
{
	for (size_t i = 0; i < this->length; i++)
	{
		*this->FindElementPtr(i) = _WIN32 ? 0UL : 0ULL;
	}
}


bool BitArray::AreAllBitsSet() const
{
	BitElement marker = _WIN32 ? ~0UL : ~0ULL;
	for (size_t i = 0; i < this->length; i++)
	{
		if (*this->FindElementPtr(i) != marker)
			return false;
	}
	return true;
}


bool BitArray::AreAllBitsClear() const
{
	BitElement marker = _WIN32 ? 0UL : 0ULL;
	for (size_t i = 0; i < this->length; i++)
	{
		if (*this->FindElementPtr(i) != marker)
			return false;
	}
	return true;
}
