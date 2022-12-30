# Memory Allocator

## Features

A universal memory allocator that re-implement the "malloc()" and "free()" allocation functions in C language and also the "new" and "delete" operator in C++ language. Memory allocator is make up of two sub-allocators: dynamic allocator and fix size allocator. Memory allocator will flexiblely decides which sub-allocator to use at runtime depending on user's demands. Memory allocator is compatible with bot 32-bit system and 64-bit system. But currently it only compatible with Windows series operating systems.

## APIs
The APIs of memory allocator includes:
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
    Dyanmic allocator is a memory allocator that designed for general memory allocation. Dynamic allocator use two linked lists to manage its memory space. One linked list for managing free menory blocks in allocator and the other for managing allocated menory blocks. Note that after the instantiation of dynamic allocator, its initial memory space will be treated as its first free memory block.

    However, using linked list to manage memory space have two shortcomings. Firstly, linked list node require extra memory space to store data like pointer of next/previous node, size of the memory block, etc. If the size of the memory that user is requesting is small, the memory overhead of creating linked list node will be relatively high. Secondly, dynamic allocator always looks for the first free memory block that can satisfy user's demand instead of the best-fit free memory block. This will leads to seriouos memory fragmentation problem.

    Solution to the first shortcoming is fix size allocator, which is specially designed for small-size allocation (see below). As for the second shortcoming, dynaimc allocator provide a "Collect()" function to merge memory fragmentations into a large memory block. To do that, dynamic allocator needs to sort the order of free linked list each time when releasing memory block.

+ ### APIs
    The APIs of dynamic allocator includes:
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
    FixSizeAllocator is a memory allocator that designed for small-size memory allocation. Fix size allocation is an optimized solution towards the memory overhead problem of dynamic allocator. The number and size of memory blocks in fix size allocator is defined by user and is fixed during runtime. 

    Instead of using linked list to manage memory space, fix size allocator takes the advantage of bit array. Bit array is an array that monitor the status of each memory block in fix size allocator. Elements of the bit array is 32-bit or 64-bit unsigned integers -- based on what architecture of current system is. Each bit in each element monitors a memory block in fix size allocator. A clear bit (0) represent an allocated block, a set bit (1) represent a free block. Compared with the linked list apporach in dynamic allocator, the memory overhead produced by bit array is negligible. What's more, fix size allocator can access any memory block by adding offset to the base address of first memory address, which is way more faster than iterating the linked list to find the expecting memory block.

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