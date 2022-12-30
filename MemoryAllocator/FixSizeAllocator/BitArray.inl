#pragma once


inline BitArray::BitArray(size_t length, size_t blockPerElement, BitElement arr)
{
	this->length = length;
	this->blockPerElement = blockPerElement;
	this->arr = arr;
}


inline BitArray::~BitArray() {}


inline BitElement* BitArray::FindElementPtr(size_t idx) const
{
	return reinterpret_cast<BitElement*>(PointerAdd(&this->arr, sizeof(BitElement) * idx));
}


inline bool BitArray::IsBitSet(size_t blockIdx) const
{
	size_t idx = blockIdx / this->blockPerElement;
	size_t bitIdx = blockIdx % this->blockPerElement;
	return *this->FindElementPtr(idx) & (1 << bitIdx);
}


inline bool BitArray::IsBitClear(size_t blockIdx) const
{
	return !this->IsBitSet(blockIdx);
}