#pragma once
#include <stdint.h>


// SECTION: System-dependant page memory allocation for arena
#ifdef __linux__
#include <sys/mman.h>
#endif
#ifdef _WIN32
// TODO: Include virutual alloc instead of stdlib when changing code from malloc to VirtualAlloc.
#include <stdlib.h>
#endif


// SECTION: Arena Allocator
struct ArenaAllocator {
    uint64_t offset_pointer;
    void* memory;
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


// SECTION: Free-List allocator (general purpose)

namespace Internal {
    struct FreeListAllocationHeader {
        size_t block_size;
        size_t padding;
    };
    struct FreeListNode {
        FreeListNode* next;
        size_t block_size;
    };
}

struct FreeListAllocator {
    // TODO: add free-list implementation
    size_t size;
    size_t used;

    void* memory;
    bool use_first; // NOTE: If this is true, then allocation will go to the first suitable node. If false, will go to best suited node instead.

    Internal::FreeListNode* head;
};

void* FreeListAlloc(FreeListAllocator* allocator, size_t allocation_size) {
    return nullptr;
}


