#pragma once
#include <stdint.h>


// SECTION: System-dependant page memory allocation for arena
#ifdef __linux__
#include <sys/mman.h>
#endif
#ifdef _WIN32
// TODO: Include virutual alloc instead of stdlib when changing code from malloc to VirtualAlloc.
#include <string.h>
#include <stdlib.h>
#endif


// SECTION: Arena Allocator
struct ArenaAllocator {
    uint64_t offset_pointer;
    void* memory;
    size_t total_size;
};

// Returns address of allocation.
void* ArenaAllocateAsset(ArenaAllocator* arena, size_t allocation_size) {
    // NOTE: increment address offset pointer.
    arena->offset_pointer += allocation_size;
    // NOTE: Returns pointer to allocated memory.
    return (void*)((uint8_t*)(arena->memory) + arena->offset_pointer - allocation_size);
}

void InitArena(ArenaAllocator* arena, uint64_t alloc_size) {
#ifdef __linux__
    arena->memory = mmap(
        nullptr, alloc_size, 
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANON | MAP_NORESERVE,
        -1, 0
    );
#endif
#ifdef _WIN32
    // TODO: Change to call VirtualAlloc() instead of malloc?
    arena->memory = malloc(alloc_size);
#endif
    arena->total_size = alloc_size;
    arena->offset_pointer = 0;
}

// NOTE: Sets all allocated values in the arena to value.
void MemsetArena(ArenaAllocator* arena, int value) {
#ifdef __linux__
    memset(arena->memory, value, arena->offset_pointer);
#endif
#ifdef _WIN32
    // TODO: Change to whatever the win32 equivalent is of memset.
    memset(arena->memory, value, arena->offset_pointer);
#endif
    arena->offset_pointer = 0;
}

// NOTE: Returns 0 if successful, -1 otherwise
int FreeArena(ArenaAllocator* arena) {
int ret = 0;
#ifdef __linux__
    ret = munmap(arena->memory, arena->total_size); 
#endif
#ifdef _WIN32
    // TODO: Change to call VirtualAlloc() instead of malloc and free?
    free(arena->memory);
#endif
    return ret;
}

// SECTION: Free-List allocator (general purpose)

struct FreeListAllocatorBlock {
    // TODO: add free-list implementation
    size_t size;
    size_t used;

    void* memory;

    FreeListAllocatorBlock* next;
};
struct FreeListAllocator {
    FreeListAllocatorBlock* free_list_head;
    void* m_start_ptr = nullptr; 
    size_t total_alloc_size = 0;
};

// NOTE: Aligns the size by the machine word.
inline size_t align(size_t n) {
    return (n + sizeof(intptr_t) - 1) & ~(sizeof(intptr_t) - 1);
}

void* FreeListAlloc(FreeListAllocator* allocator, size_t allocation_size) {
    allocation_size = align(allocation_size);
    FreeListAllocatorBlock block = {0};
    block.size = allocation_size;
    block.used = true;
    if (allocator->head == nullptr) {
        allocator->head = &block;
    }
    return nullptr;
}


void InitFreeList(FreeListAllocator* arena, uint64_t total_alloc_size) {
    // NOTE: Reset the memory pointer and unmap the memory if it is already mapped
    // and allocate new memory, setting the memeory pointer. 
#ifdef __linux__
    if (m_start_ptr != nullptr) {
        munmap(m_start_ptr, arena->total_alloc_size);
        m_start_ptr = nullptr;
    }
    m_start_ptr = mmap(nullptr, total_alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON | MAP_NORESERVE, -1, 0);
#endif
#ifdef _WIN32
    // TODO: Change to call VirtualAlloc() instead of malloc?
    if (m_start_ptr != nullptr) {
        free(m_start_ptr);
        m_start_ptr = nullptr;
    }
    m_start_ptr = malloc(total_alloc_size);
#endif

    // NOTE: Save the total allocation size
    arena->total_alloc_size = total_alloc_size; 
    
    // NOTE: Reset the freelist
    // and place it at the start of the allocated memeory pointer location
    arena->mem_used = 0;
    arena->mem_peak = 0;
    FreeListAllocatorBlock* first_block = (FreeListAllocatorBlock*)m_start+ptr;
    first_block->size = total_alloc_size;
    first_block->next = nullptr;
    // TODO: FINISH IMPLEMENTATION LIKE IN THE GITHUB:
    // https://github.com/mtrebi/memory-allocators/blob/master/src/FreeListAllocator.cpp

}

