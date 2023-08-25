#pragma once
#include <stdio.h>
#include <string.h>
#include <stdint.h>


#include "tilemap.h"
#include "entity.h"

const char level_directory[] = "levels";
const char level_filetype_extension[] = "level";


void WriteEntityState(
    Entity* entity,
    FILE* file_p
) {
    // NOTE: General attributes
    fwrite(uint32_t(entity->id),                sizeof(uint32_t), 1, file_p);
    fwrite(uint32_t(entity->active),            sizeof(uint32_t), 1, file_p);
    fwrite(uint32_t(entity->position.x),        sizeof(uint32_t), 1, file_p);
    fwrite(uint32_t(entity->position.y),        sizeof(uint32_t), 1, file_p);
    
    // NOTE: Component flags
    fwrite(uint32_t(entity->player.active),     sizeof(uint32_t), 1, file_p);
    fwrite(uint32_t(entity->mover.active),      sizeof(uint32_t), 1, file_p);
    fwrite(uint32_t(entity->emitter.active),    sizeof(uint32_t), 1, file_p);
    fwrite(uint32_t(entity->receiver.active),   sizeof(uint32_t), 1, file_p);
    fwrite(uint32_t(entity->door.active),       sizeof(uint32_t), 1, file_p);
    fwrite(uint32_t(entity->endgoal.active),    sizeof(uint32_t), 1, file_p);

    // NOTE: Emitter
    fwrite(uint32_t(entity->emitter.direction), sizeof(uint32_t), 1, file_p);
    // TODO: Color
    
    // NOTE: Receiver

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
    printf("%s\n", filepath);

    // NOTE: open file
    FILE* file_p = fopen(level_name, "wb");

    if (file_p == nullptr) {
        printf("LOG::SaveLevelState: Cannot open file %s", filepath);
        return;
    }

    // NOTE: Write header:
    fwrite(tilemap->width, sizeof(uint32_t), 1, file_p); // width
    fwrite(tilemap->height, sizeof(uint32_t), 1, file_p); // height
    fwrite(uint32_t(num_floor_tile_types), sizeof(uint32_t), 1, file_p); // num_of_floor_tile_variations
    fwrite(uint32_t(num_entities), sizeof(uint32_t), 1, file_p); // number of entities

    // NOTE: Write the tilemap
    for (int y = 0; y < tilemap->height; ++y) {
        for (int x = 0; x < tilemap->width; ++x) {
            fwrite(uint16_t(tilemap->map[y * tilemap->width + x]), sizeof(uint16_t), 1, file_p);
        }
    }

    // NOTE: Write the entity states:



}