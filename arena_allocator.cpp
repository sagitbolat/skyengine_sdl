#pragma once
#include "skyengine.h"

namespace Arena {
    // Returns address of allocation.
    void* AllocateAsset(GameMemory* memory, size_t allocation_size) {
    	// NOTE: increment address pointer.
        memory->transient_current_address += allocation_size;
        // NOTE: Returns pointer to allocated memory.
        return (void*)((uint8_t*)(memory->transient_storage) + memory->transient_current_address - allocation_size);
    }
}
