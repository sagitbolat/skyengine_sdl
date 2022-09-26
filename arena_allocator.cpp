#include "skyengine.h"

namespace Arena {
    
    size_t AllocateAsset(GameMemory* memory, size_t allocation_size) {
        memory->transient_current_address += allocation_size;
        //returns address of allocation.
        return (uint8_t*)(memory->transient_storage) + memory->transient_current_address - allocation_size;
    }
}
