#pragma once
#include "skyengine.h"


// SECTION: Arena Allocator
struct ArenaAllocator {
    size_t offset_pointer;
    void* memory;
}

// Returns address of allocation.
void* ArenaAllocateAsset(ArenaAllocator* arena, size_t allocation_size) {
    // NOTE: increment address offset pointer.
    arena->offset_pinter += allocation_size;
    // NOTE: Returns pointer to allocated memory.
    return (void*)((uint8_t*)(arena->memory) + arena->offset_pointer - allocation_size);
}

// SECTION: Free-List allocator (general purpose)

struct FreeListAllocator {
    // TODO: add free-list implementation
    void* memory;
}

void* FreeListAlloc(FreeListAllocator* allocator, size_t allocation_size) {
    return nullptr
}


