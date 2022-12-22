#include "SDL_sky.cpp"


#include "bmp.cpp"
#include "simplex_noise.cpp"

#include "limits.h"
#include "float.h"
int ArrMin(int* arr, size_t len) {
    int imin = INT_MAX;
    for (size_t i = 0; i < len; ++i) {
        if (arr[i] < imin) {
	    imin = arr[i];
        }
    }
    return imin; 
}
int ArrMax(int* arr, size_t len) {
    int imax = INT_MIN;
    for (size_t i = 0; i < len; ++i) {
        if (arr[i] > imax) {
	    imax = arr[i];
        }
    }
    return imax; 
}
int ArrMin(float* arr, size_t len) {
    float imin = FLT_MAX;
    for (size_t i = 0; i < len; ++i) {
        if (arr[i] < imin) {
	    imin = arr[i];
        }
    }
    return imin; 
}
int ArrMax(float* arr, size_t len) {
    float imax = FLT_MIN;
    for (size_t i = 0; i < len; ++i) {
        if (arr[i] > imax) {
	    imax = arr[i];
        }
    }
    return imax; 
}



int cells_width = 1280;
int cells_height = 720;
int* cells;



void GenerateCells(int octaves) {
    for (int y = 0; y < cells_height; ++y) {
    	for (int x = 0; x < cells_width; ++x) {
	    int val = 0;
    	    if (Noise::simplex_fractal(octaves, (float)x * 0.001f, (float)y * 0.001f) > 0) {
		val = 255;
	    }
	    cells[y * cells_width + x] = val;
	}	
    }
}

void Init(int* w, int* h) {
    *w = 1280;
    *h = 720;
    return;
}


void Awake(GameMemory* gm) {
    cells = (int*)ArenaAllocateAsset(&gm->permanent_storage, sizeof(int) * cells_width * cells_height);
}

void Start(GameState* gs, KeyboardState* ks) {
    GenerateCells(1);
    int imin = ArrMin(cells, cells_width * cells_height);
    int imax = ArrMax(cells, cells_width * cells_height);
    printf("Min: %d\n", imin);
    printf("Max: %d\n", imax);
}

void Update(GameState* gs, KeyboardState* ks, int dt) {
    static int octaves = 1;
    if (ks->state.D == 1 && ks->prev_state.D == 0) {
    	octaves += 1;
	printf("Current octaves: %d\n", octaves);
    } else if (ks->state.A == 1 && ks->prev_state.A == 0) {
    	octaves -= 1;
	printf("Current octaves: %d\n", octaves);
    }
    GenerateCells(octaves);
	
    for (int y = 0; y < cells_height; ++y) {
    	for (int x = 0; x < cells_width; ++x) {
	    int val = cells[y * cells_width + x];
	    if (val > 1 || val < -1) {
	    	//printf("%f\n", val);
	    } 
	    DrawPixel(graphics_buffer, (uint8_t) (val), (uint8_t) (val), (uint8_t) (val), x, y);	
	
	}
    }
}

void UserFree() {

}
