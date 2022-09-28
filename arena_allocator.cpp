#include "skyengine.h"

namespace Arena {
    
    void* AllocateAsset(GameMemory* memory, size_t allocation_size) {
        memory->transient_current_address += allocation_size;
        //returns address of allocation.
        return (void*)((uint8_t*)(memory->transient_storage) + memory->transient_current_address - allocation_size);
    }
}
