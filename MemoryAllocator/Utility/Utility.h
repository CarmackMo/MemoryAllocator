#pragma once
#include <stdlib.h>
#include <stdio.h>

namespace Utility
{

inline void* PointerAdd(const void* ptr, size_t size)
{
	return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) + size);
}

inline void* PointerAdd(const void* ptr1, const void* ptr2)
{
	return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr1) + reinterpret_cast<uintptr_t>(ptr2));
}

inline void* PointerSub(const void* ptr, size_t size)
{
	return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) - size);
}

inline void* PointerSub(const void* ptr1, const void* ptr2)
{
	return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr1) - reinterpret_cast<uintptr_t>(ptr2));
}

}
