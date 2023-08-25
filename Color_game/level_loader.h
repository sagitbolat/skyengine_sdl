#pragma once
#include <stdio.h>
#include <string.h>
#include <stdint.h>


#include "tilemap.h"
#include "entity.h"

const char level_directory[] = "levels";
const char level_filetype_extension[] = "level";


void WriteUint32(int i, FILE* file_p) {
    uint32_t ui32 = uint32_t(i);
    fwrite(&ui32, sizeof(uint32_t), 1, file_p);
}

void WriteEntityState(
    Entity* entity,
    FILE* file_p
) {
    // NOTE: General attributes
    WriteUint32(entity->id,         file_p); 
    WriteUint32(entity->active,     file_p); 
    WriteUint32(entity->position.x, file_p); 
    WriteUint32(entity->position.y, file_p); 
    
    // NOTE: Component flags
    WriteUint32(entity->player.active,      file_p); 
    WriteUint32(entity->movable.active,     file_p); 
    WriteUint32(entity->emitter.active,     file_p); 
    WriteUint32(entity->receiver.active,    file_p); 
    WriteUint32(entity->door.active,        file_p); 
    WriteUint32(entity->endgoal.active,     file_p); 
    
    // NOTE: Emitter
    WriteUint32(int(entity->emitter.direction), file_p);
    WriteUint32(int(entity->emitter.emission_color.r), file_p);
    WriteUint32(int(entity->emitter.emission_color.g), file_p);
    WriteUint32(int(entity->emitter.emission_color.b), file_p);
    WriteUint32(int(entity->emitter.emission_color.a), file_p);
    
    // NOTE: Receiver
    WriteUint32(int(entity->receiver.accepted_color.r), file_p);
    WriteUint32(int(entity->receiver.accepted_color.g), file_p);
    WriteUint32(int(entity->receiver.accepted_color.b), file_p);
    WriteUint32(int(entity->receiver.accepted_color.a), file_p);

    // NOTE: Door
    WriteUint32(int(entity->door.require_all_to_unlock), file_p);
    WriteUint32(int(entity->door.num_connected_receivers), file_p);
    for (int i = 0; i < MAX_CONNECTED_RECIEVERS; ++i) {
        WriteUint32(int(entity->door.connected_receiver_ids[i]), file_p);
    }
    
}

void SaveLevelState(
    const char* level_name, 
    Tilemap* tilemap, 
    int num_floor_tile_types, 
    Entity* entity_array, 
    int num_entities
) {
    // NOTE: construct level filepath from name:
    char filepath[256] = {0};
    snprintf(filepath, sizeof(filepath), "%s/%s.%s", level_directory, level_name, level_filetype_extension);
    printf("LOG::SaveLevelState: Saving level to file: %s\n", filepath);

    // NOTE: open file
    FILE* file_p = fopen(filepath, "wb");

    if (file_p == nullptr) {
        printf("LOG::SaveLevelState: Cannot open file %s", filepath);
        return;
    }


    uint32_t width32         = uint32_t(tilemap->width);
    uint32_t height32        = uint32_t(tilemap->height);
    uint32_t num_floor32     = uint32_t(num_floor_tile_types);
    uint32_t num_entities32  = uint32_t(num_entities);
    // NOTE: Write header:
    fwrite(&width32,         sizeof(uint32_t), 1, file_p); // width
    fwrite(&height32,        sizeof(uint32_t), 1, file_p); // height
    fwrite(&num_floor32,     sizeof(uint32_t), 1, file_p); // num_of_floor_tile_variations
    fwrite(&num_entities32,  sizeof(uint32_t), 1, file_p); // number of entities

    // NOTE: Write the tilemap
    for (int y = 0; y < tilemap->height; ++y) {
        for (int x = 0; x < tilemap->width; ++x) {

            uint16_t tile = uint16_t(tilemap->map[y * tilemap->width + x]);
            fwrite(&tile, sizeof(uint16_t), 1, file_p);
        }
    }

    // NOTE: Write the entity states:
    for (int e = 0; e < num_entities; ++e) {
        WriteEntityState(&entity_array[e], file_p);
    }

    fclose(file_p);
    printf("LOG::SaveLevelState: Finished saving level to file: %s\n", filepath);
    printf("                     Num Entities Written: %d", num_entities);
}