#include "SDL_sky.cpp"
#include "sky_random.cpp"
// SECTION: Generation algorithm

#define TILE_LAND 1
#define TILE_WATER 0


struct IslandState {
    uint32_t tiles[256 * 144];

};

int square_size = 5; // NOTE: Size of square tile in pixels 

// NOTE: returns the number of land tiles around the tile at [x, y]
static int NumNeighbors(IslandState* game_state, int x, int y, int max_x, int max_y) {
    int count = 0;
    
    for (int c = x - 1; c <= x + 1; ++c) {
        for (int r = y - 1; r <= y + 1; ++r) {
            
            if ((c == x && r == y) || 
                (r < 0 || c < 0) ||
                (r >= max_y || c >= max_x)) {
                continue;
            }
            
            int i = (max_x * r) + c;

            if (game_state->tiles[i] == TILE_LAND) {
                ++count;
            }
        }
    }
    return(count);
}

static uint32_t IslandRuleset(uint32_t prev_state, int num_neighbors) {
    if (num_neighbors > 4) {
        return TILE_LAND;
    } else if (num_neighbors < 4) {
        return TILE_WATER;
    } else {
        return prev_state;
    }
}

// NOTE: Generates the intial tilemap using plate tectonic algorithm.
static void GenerateTileMap(IslandState* game_state) {
    // STEP 1: populate with random noise
    for (int y = 0; y < 144; ++y) {
        for (int x = 0; x < 256; ++x) {
            
            uint64_t chance_for_water = 45;
            uint32_t tile_id = TILE_LAND;
            if (xorshift128plus::sky_rand() % 100 < chance_for_water) {
                tile_id = TILE_WATER;            
            } 
            game_state->tiles[y * 256 + x] = tile_id;

        }
    }     


    // STEP 2: collapse into a continental map with cellular automaton
    uint32_t* new_tiles = (uint32_t*)malloc(sizeof(uint32_t) * 144 * 256);
    for (int k = 0; k < 5; ++k) {
        for (int y = 0; y < 144; ++y) {
            for (int x = 0; x < 256; ++x) {
                int i = y * 256 + x;
                uint32_t prev_state = game_state->tiles[i];
                int num_neighbors = NumNeighbors(game_state, x, y, 256, 144); 
                uint32_t new_state = IslandRuleset(prev_state, num_neighbors);
                new_tiles[i] = new_state;        
            }
        } 
        for (int y = 0; y < 144; ++y) {
            for (int x = 0; x < 256; ++x) {
                int i = 256 * y + x;
                game_state->tiles[i] = new_tiles[i];        
            }
        }
    }
    free(new_tiles);
}

static void SkyInit() {
    return;
}

static void Start(GameState* game_state, KeyboardState* keyboard_state) {
    IslandState* island_state = (IslandState*) game_state;
    GenerateTileMap(island_state);
    return;
}

static void Update(GameState* game_state, KeyboardState* keyboard_state) {
    IslandState* island_state = (IslandState*) game_state;
    for (int y = 0; y < 144; ++y) {
        for (int x = 0; x < 256; ++x) {
            uint32_t tile_id = island_state->tiles[y * 256 + x];
            uint8_t red = 0;
            uint8_t green = 0;
            uint8_t blue = 155;
            
            if (tile_id == 1) {
                red = 0;
                green = 155;
                blue = 0;
            }

            int min_x = x * square_size;
            int min_y = y * square_size;
            DrawRectangle(red, green, blue, min_x, min_y, min_x + square_size, min_y + square_size);
        }  
    }
    return;
}