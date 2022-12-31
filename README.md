# Memory Allocator

## Introduction
MemoryAllocator is a hardware-oriented memory allocator. That is, given the starting address and the size of a physical memory space, MemoryAllocator manages memory allocations on this memory space without using extra memory space and external APIs. MemoryAllocator acts like the `malloc()` and `free()` function in C language and the `new` and `delete` operator in C++ language.


## Features
+ MemoryAllocator does not rely on *C++ Standard Library (STD)* and has mininum dependency on *C Runtime Library (CRT)* that is necessary for its normal operation (e.g. `assert.h`, `inttypes.h`, `stdlib.h`, etc.) Advance data structures and logics are implemented using basic C/C++ syntax instead of using external libraries.
+ MemoryAllocator is campatible with both 32-bit system and 64-bit system. It is also compatible with most of the operating systems. MemoryAllocator can be easily deployed on different systems.
+ MemoryAllocator is made up of several independent sub-systems: one dynamic allocator and several fix size allocators (See below for more detail of dynamic allocator and fix size allocator) By doing so, the coupling of each sub-system are highly reduced, which also reduce the risk of overall crash cause by the errors from one of the sub-system. In this project, MemoryAllocator has one dynamic allocator, one 16-bit fix size allocator, one 32-bit fix size allocator and one 96-bit fix size allocator.
+ The structure of MemoryAllocator is like: ![MemoryAllocator Structure](Images/MemoryAllocator.png)
  

## APIs
The APIs of MemoryAllocator includes:
  ```cpp
    bool InitializeMemoryAllocator(void * i_pHeapMemory, size_t i_sizeHeapMemory);

    void DestroyMemoryAllocator();

    void* Alloc(size_t size);

    void Free(void* ptr);

    void Collect();

    void* operator new(size_t size);

    void* operator new[](size_t size);

    void operator delete(void* ptr);

    void operator delete[](void* ptr);
  ```




## Dynamic Allocator
+ ### Features
    DyanmicAllocator is a memory allocator that designed for general memory allocation. DynamicAllocator use two linked lists to manage its memory space. One linked list for managing free menory blocks in allocator and the other for managing allocated menory blocks. Note that after the instantiation of DynamicAllocator, its initial memory space will be treated as its first free memory block.

    However, using linked list to manage memory space have two shortcomings. Firstly, linked list node require extra memory space to store data like pointer of next/previous node, size of the memory block, etc. If the size of the memory that user is requesting is small, the memory overhead of creating linked list node will be relatively high. Secondly, DynamicAllocator always looks for the first free memory block that can satisfy user's demand instead of the best-fit free memory block. This will leads to seriouos memory fragmentation problem.

    Solution to the first shortcoming is fix size allocator, which is specially designed for small-size allocation (see below). As for the second shortcoming, dynaimc allocator provide a "Collect()" function to merge memory fragmentations into a large memory block. To do that, DynamicAllocator needs to sort the order of free linked list each time when releasing memory block.

    The structure of DynamicAllocator is like: ![DynaimcAllocator Structure](Images/DynamicAllocator.png)

    The structure of each memory block in DynamicAllocator is like: ![Memory Block Structure](Images/MemoryBlock.png)


+ ### APIs
    The APIs of DynamicAllocator includes:
  ```cpp
    void* Alloc(size_t size);

    void* Alloc(size_t size, unsigned int alignment);

    bool Free(void* ptr);

    bool Contains(void* ptr);
    
    bool IsAllocated(void* ptr);

    void Collect();

    void Destroy();

    DynamicAllocator* CreateDynamicAllocator(void* baseAddr, size_t size);
  ```


## Fix Size Allocator
+ ### Features
    FixSizeAllocator is a memory allocator that designed for small-size memory allocation. Fix size allocation is an optimized solution towards the memory overhead problem of DynamicAllocator. The number and size of memory blocks in FixSizeAllocator is defined by user and is fixed during runtime. 

    Instead of using linked list to manage memory space, FixSizeAllocator takes the advantage of bit array. Bit array is an array that monitor the status of each memory block in FixSizeAllocator. Elements of the bit array is 32-bit or 64-bit unsigned integers -- based on what architecture of current system is. Each bit in each element monitors a memory block in FixSizeAllocator. A clear bit (0) represent an allocated block, a set bit (1) represent a free block. Compared with the linked list apporach in DynamicAllocator, the memory overhead produced by bit array is negligible. What's more, FixSizeAllocator can access any memory block by adding offset to the base address of first memory address, which is way more faster than iterating the linked list to find the expecting memory block.

    The structure of FixSizeAllocator is like: ![FixSizeAllocator Structure](Images/FixSizeAllocator.png)

+ ### APIs
    The APIs of fix allocator includes:
  ```cpp
    void* Alloc();

    bool Free(void* ptr);

    bool Contains(void* ptr);

    bool IsAllocated(void* ptr);

    void Destroy();

    FixSizeAllocator* CreateFixSizeAllocator(void* baseAddr, size_t blockNum, size_t blockSize, size_t heapSize);
  ```