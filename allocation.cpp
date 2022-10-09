#pragma once
#include <stdint.h>
// SECTION: Arena Allocator
struct ArenaAllocator {
    size_t offset_pointer;
    void* memory;
};

// Returns address of allocation.
void* ArenaAllocateAsset(ArenaAllocator* arena, size_t allocation_size) {
    // NOTE: increment address offset pointer.
    arena->offset_pointer += allocation_size;
    // NOTE: Returns pointer to allocated memory.
    return (void*)((uint8_t*)(arena->memory) + arena->offset_pointer - allocation_size);
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


