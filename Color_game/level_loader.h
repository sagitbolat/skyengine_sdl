#pragma once
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>


#include "tilemap.h"
#include "entity.h"

const char level_directory[] = "levels";
const char level_filetype_extension[] = "level";


void WriteUint32(int i, FILE* file_p) {
    uint32_t ui32 = uint32_t(i);
    fwrite(&ui32, sizeof(uint32_t), 1, file_p);
}

int ReadUint32(FILE* file_p) {
    int i;
    fread(&i, sizeof(uint32_t), 1, file_p);
    return i;
}

void WriteEntityState(
    Entity* entity,
    FILE* file_p
) {
    // NOTE: General attributes
    WriteUint32(entity->id,                 file_p); 
    WriteUint32(entity->active,             file_p); 
    WriteUint32(entity->position.x,         file_p); 
    WriteUint32(entity->position.y,         file_p); 
    WriteUint32(int(entity->entity_type),   file_p);

    // NOTE: Component flags
    WriteUint32(entity->player.active,      file_p); 
    WriteUint32(entity->movable.active,     file_p); 
    WriteUint32(entity->emitter.active,     file_p); 
    WriteUint32(entity->receiver.active,    file_p); 
    WriteUint32(entity->door.active,        file_p); 
    WriteUint32(entity->endgoal.active,     file_p); 
    WriteUint32(entity->button.active,      file_p); 
    
    // NOTE: Emitter
    WriteUint32(int(entity->emitter.direction),        file_p);
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
    WriteUint32(int(entity->door.open_by_default), file_p);
    WriteUint32(int(entity->door.num_connected_activators), file_p);
    for (int i = 0; i < MAX_CONNECTED_ACTIVATORS; ++i) {
        WriteUint32(int(entity->door.connected_activators_ids[i]), file_p);
    }

    // NOTE: Teleporter
    WriteUint32(int(entity->teleporter.active), file_p);
    WriteUint32(int(entity->teleporter.color.r), file_p);
    WriteUint32(int(entity->teleporter.color.g), file_p);
    WriteUint32(int(entity->teleporter.color.b), file_p);
    WriteUint32(int(entity->teleporter.color.a), file_p);
    WriteUint32(int(entity->teleporter.connected_teleporter_id), file_p);
    
    // NOTE: Color Changer
    WriteUint32(int(entity->color_changer.active), file_p);
    WriteUint32(int(entity->color_changer.color.r), file_p);
    WriteUint32(int(entity->color_changer.color.g), file_p);
    WriteUint32(int(entity->color_changer.color.b), file_p);
    WriteUint32(int(entity->color_changer.color.a), file_p);
    WriteUint32(int(entity->color_changer.color_mode), file_p);
    
}
void ReadEntityState(Entity* entity, EntityMap* entity_map, const Sprite* sprites, FILE* file_p) {
    // NOTE: General attributes
    int id         = ReadUint32(file_p); 
    int active     = ReadUint32(file_p); 
    int pos_x      = ReadUint32(file_p); 
    int pos_y      = ReadUint32(file_p); 
    int entity_type = ReadUint32(file_p); 
    
    // NOTE: Component flags
    int player_active   = ReadUint32(file_p); 
    int movable_active  = ReadUint32(file_p); 
    int emitter_active  = ReadUint32(file_p); 
    int receiver_active = ReadUint32(file_p); 
    int door_active     = ReadUint32(file_p); 
    int endgoal_active  = ReadUint32(file_p);
    int button_active   = ReadUint32(file_p); 
    



    // NOTE: Emitter
    EntityComponentEmitter::DIRECTION_ENUM emitter_direction = (EntityComponentEmitter::DIRECTION_ENUM)ReadUint32(file_p);
    Color emitter_emission_color = {0};
    emitter_emission_color.r = ReadUint32(file_p);
    emitter_emission_color.g = ReadUint32(file_p);
    emitter_emission_color.b = ReadUint32(file_p);
    emitter_emission_color.a = ReadUint32(file_p);
    
    // NOTE: Receiver
    Color receiver_accepted_color = {0};
    receiver_accepted_color.r = ReadUint32(file_p);
    receiver_accepted_color.g = ReadUint32(file_p);
    receiver_accepted_color.b = ReadUint32(file_p);
    receiver_accepted_color.a = ReadUint32(file_p);

    // NOTE: Door
    int door_open_by_default = ReadUint32(file_p);
    int door_connected_activators = ReadUint32(file_p);
    int connected_activator_ids[MAX_CONNECTED_ACTIVATORS];
    for (int i = 0; i < MAX_CONNECTED_ACTIVATORS; ++i) {
        connected_activator_ids[i] = ReadUint32(file_p);
    }
    // NOTE: Teleporter
    int teleporter_active = ReadUint32(file_p);
    Color teleporter_color = {0};
    teleporter_color.r = ReadUint32(file_p);
    teleporter_color.g = ReadUint32(file_p);
    teleporter_color.b = ReadUint32(file_p);
    teleporter_color.a = ReadUint32(file_p);
    int teleporter_connected_id = ReadUint32(file_p);


    // NOTE: Color Changer
    int color_changer_active = ReadUint32(file_p);
    Color color_changer_color = {0};
    color_changer_color.r = ReadUint32(file_p);
    color_changer_color.g = ReadUint32(file_p);
    color_changer_color.b = ReadUint32(file_p);
    color_changer_color.a = ReadUint32(file_p);
    EntityComponentColorChanger::COLOR_MODE color_changer_color_mode = (EntityComponentColorChanger::COLOR_MODE)ReadUint32(file_p);

    // NOTE: Main inits:
    entity->active = active;
    if (player_active) {
        PlayerInit(
            entity, 
            id, 
            sprites[0], 
            sprites[1], 
            sprites[2], 
            sprites[3], 
            sprites[4], 
            {pos_x, pos_y}
        );
        entity_map->SetID(pos_x, pos_y, 1, id);
    } else if (emitter_active) {
        EmitterInit(
            entity, 
            id, 
            sprites[7], 
            sprites[8], 
            sprites[9], 
            {pos_x, pos_y}, 
            emitter_emission_color, 
            movable_active, 
            emitter_direction
        );
        entity_map->SetID(pos_x, pos_y, 1, id);
    } else if (receiver_active) {
        ReceiverInit(
            entity, 
            id, 
            sprites[10], 
            sprites[11], 
            sprites[12],
            {pos_x, pos_y}, 
            receiver_accepted_color, 
            movable_active
        );
        entity_map->SetID(pos_x, pos_y, 1, id);
    } else if (endgoal_active) {    
        EndgoalInit(entity, id, sprites[17], {pos_x, pos_y});
        entity_map->SetID(pos_x, pos_y, 0, id);
    } else if (door_active) {
        DoorInit(
            entity,
            id,
            sprites[13],
            sprites[14],
            sprites[15],
            sprites[16],
            {pos_x, pos_y},
            (bool)door_open_by_default
        );
        entity_map->SetID(pos_x, pos_y, 0, id);
    } else if (button_active) {
        ButtonInit(entity, id, sprites[18], sprites[19], {pos_x, pos_y});
        entity_map->SetID(pos_x, pos_y, 0, id);
    } else if (entity_type == Entity::ENTITY_TYPE_ENUM::TELEPORTER) {
        TeleporterInit(entity, id, sprites[20], teleporter_color, teleporter_connected_id, {pos_x, pos_y});
        entity_map->SetID(pos_x, pos_y, 0, id);
    } else if (color_changer_active) {
        ColorChangerInit(entity, id, sprites[21], sprites[22], sprites[23], color_changer_color, color_changer_color_mode, {pos_x, pos_y});
        entity_map->SetID(pos_x, pos_y, 1, id);
    }else if (entity_type == Entity::ENTITY_TYPE_ENUM::PUSH_BLOCK) {
        PushblockInit(entity, id, sprites[5], {pos_x, pos_y});
        entity_map->SetID(pos_x, pos_y, 1, id);
    } else if (entity_type == Entity::ENTITY_TYPE_ENUM::STATIC_BLOCK) {
        StaticBlockInit(entity, id, sprites[6], {pos_x, pos_y});
        entity_map->SetID(pos_x, pos_y, 1, id);
    }

    // NOTE: Connect Doors
    entity->door.num_connected_activators = door_connected_activators;
    for (int i = 0; i < MAX_CONNECTED_ACTIVATORS; ++i) {
        entity->door.connected_activators_ids[i] = connected_activator_ids[i]; 
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
        printf("LOG::SaveLevelState: Cannot open file %s\n", filepath);
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
    printf("                     Num Entities Written: %d\n", num_entities);
}

struct LevelStateInfo {
    int num_floor_tile_types;
    int num_entities;
};

LevelStateInfo ReadLevelState(
    const char* level_name, 
    Tilemap* tilemap, 
    Entity** entity_array,
    EntityMap* entity_map,
    const Sprite* all_sprites,
    int* num_movable_entities 
) {

    // NOTE: reset num_movable_entities when loading new level:
    if (num_movable_entities != nullptr) (*num_movable_entities) = 0;

    // NOTE: construct level filepath from name:
    char filepath[256] = {0};
    snprintf(filepath, sizeof(filepath), "%s/%s.%s", level_directory, level_name, level_filetype_extension);
    //printf("LOG::ReadLevelState: Reading level from file: %s\n", filepath);

    // NOTE: open file
    FILE* file_p = fopen(filepath, "rb");

    if (file_p == nullptr) {
        //printf("LOG::ReadLevelState: Cannot open file %s\n", filepath);
        return {-1, -1};
    }


    // NOTE: Read header:
    tilemap->width    = ReadUint32(file_p);
    tilemap->height   = ReadUint32(file_p);
    int num_floor    = ReadUint32(file_p);
    int num_entities = ReadUint32(file_p);


    // NOTE: allocate or reallocate tilemap
    if (tilemap->map == nullptr) {
        tilemap->map = (int*)calloc(tilemap->width * tilemap->height, sizeof(int));
    } else {
        tilemap->map = (int*)realloc(tilemap->map, sizeof(int) * tilemap->width * tilemap->height);
    }    
    // NOTE: Read the tilemap
    for (int y = 0; y < tilemap->height; ++y) {
        for (int x = 0; x < tilemap->width; ++x) {
            uint16_t tile = 0;
            fread(&tile, sizeof(uint16_t), 1, file_p);
            tilemap->map[y * tilemap->width + x] = tile;
        }
    }


    // NOTE: allocate or reallocate entity_array
    // FIXME: Replace MAX_ENTITIES with num_entities to not take up as much space.
        // THis is only here so I can spawn entities in after loading a level
    if (*entity_array == nullptr) {
        *entity_array = (Entity*)malloc(sizeof(Entity) * MAX_ENTITIES);
    } else {
        free(*entity_array);
        *entity_array = (Entity*)calloc(sizeof(Entity), MAX_ENTITIES);
    }

    // NOTE: allocate or reallocate entity_map

    if (entity_map == nullptr) {
        entity_map->width     = tilemap->width;
        entity_map->height    = tilemap->height;
        entity_map->depth     = 2;
        entity_map->map       = (int*)malloc(sizeof(int) * tilemap->height * tilemap->width * entity_map->depth);
        for (int i = 0; i < tilemap->height * tilemap->width * entity_map->depth; ++i) entity_map->map[i] = -1;
    } else {
        entity_map->width     = tilemap->width;
        entity_map->height    = tilemap->height;
        entity_map->depth     = 2;
        entity_map->map       = (int*)realloc(entity_map->map, sizeof(int) * tilemap->height * tilemap->width * entity_map->depth);
        for (int i = 0; i < tilemap->height * tilemap->width * entity_map->depth; ++i) entity_map->map[i] = -1;
    }

    // NOTE: Read the entity states:
    for (int e = 0; e < num_entities; ++e) {
        ReadEntityState(&(*entity_array)[e], entity_map, all_sprites, file_p);
        
        if (num_movable_entities == nullptr) continue;
        if ((*entity_array)[e].active && (*entity_array)[e].movable.active) {
            ++(*num_movable_entities);
        }

    }

    fclose(file_p);
    printf("LOG::ReadLevelState: Finished reading level from file: %s\n", filepath);
    printf("                     Num Entities Loaded: %d\n", num_entities);

    return LevelStateInfo{num_floor, num_entities};
}
