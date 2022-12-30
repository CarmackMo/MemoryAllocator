#pragma once
#include <inttypes.h>
#include <intrin.h>
#include "../Utility/Utility.h"


using namespace Utility;


#if _WIN32
typedef uint32_t BitElement;
#else
typedef uint64_t BitElement;
#endif // WIN32


/**
* @brief BitArray is an array that monitor the status of each memory block in fix size
*		 allocator. Elements of the bit array is 32-bit or 64-bit unsigned integers -- based
*		 on the architecture of current system (32-bit or 64-bit system). Each bit in each 
*		 element monitors a memory block in fix size allocator. A clear bit (0) represent an 
*		 allocated block, a set bit (1) represent a free block. Note that BitArray use a single 
*		 integer to represent an array, compiler still treats "bit array" as an single 
*		 integer. Therefore, "array" will not has range protection as regular array does. User 
*		 needs to manually implement array range protection. 
*
* @param length -- The number of elements in bit array;
* @param blockPerElement -- How many blocks does each element monitoring;
* @param arr -- Entry of the bit array. arr is the first element of the bit array. The address 
*				of arr is count as the starting address of the bit array;
*/
class BitArray
{
public:
	size_t length;
	size_t blockPerElement;
	BitElement arr;


	inline BitArray(size_t length = 0, size_t blockPerElement = 0, BitElement arr = 0);
	inline ~BitArray();

	inline BitElement* FindElementPtr(size_t idx) const;

	/**
	* @brief Go through all elements in the bit array, and scan from the least significant
	*		 bit to the most significant bit of each element to find the first allocated 
	*		 bit (clear bit) in the bit array.
	* 
	* @param &outIdx -- Result of the function: index of the first allocated bit in 
	*		 bit array;
	*
	* @return If a allocated bit is found, return true and result is assigned to parameter
	*		  "outIdx". Otherwise, return false;
	*/
	bool FindFirstAllocateBit(size_t& outIdx) const;
	bool FindFirstFreeBit(size_t& outIdx) const;

	void SetBit(size_t blockIdx);
	void ClearBit(size_t blockIdx);

	void SetAllBits();
	void ClearAllBits();

	inline bool IsBitSet(size_t blockIdx) const;
	inline bool IsBitClear(size_t blockIdx) const;

	bool AreAllBitsSet() const;
	bool AreAllBitsClear() const;
};


/**
* @brief Instantiate a BitArray instance in the designated memory space. It is done by
*		 manually assigning a block of unoccupied memory space to store the BitArray
*		 instance and telling the compiler to treat that memory space as a BitArray.
* 
* @param baseAddr -- The starting address of the free memory space that is going to 
*		 store the BitArray instance.
* @param blockNum -- The number of memory blocks that bit array is going to monitor.
* @param initToOne -- A trigger to determine whether the bit array should be initialized
*		 to all set.
* 
* @return The address of BitArray instance. It is equals to the parameter "baseAddr"
*/
BitArray* CreateBitArray(void* baseAddr, size_t blockNum, bool initToOne);


#include "BitArray.inl"
