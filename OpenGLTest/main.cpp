#include "Utils.cpp"

#include <stdint.h>
#include <stdio.h>

#include "shader.h"


int main(int argc, char* argv[]) {

    // NOTE: Init the SDL window and the OpenGL context.
    WindowState window = InitWindowContext(600, 600, "SkyEngine App", {0.05, 0.0, 0.15, 1.0});



    Sprite sprite = LoadSprite("Test_tile.bmp", "../tex_shader.vs", "../tex_shader.fs");
    
    Transform transform = {0.0f};
    transform.position = {0.0f, 0.0f, 0.0f};
    transform.rotation = {0.0f, 0.0f, 0.0f};
    transform.scale = {1.0f, 1.0f, 1.0f};

    Camera camera = {0};
    camera.position = {0.0f, 0.0f, 3.0f};
    camera.up_direction = {0.0f, 0.1f, 0.0f};
    camera.look_target = {0.0f, 0.0f, 0.0f};
    camera.near_plane = 0.1f;
    camera.far_plane = 1000.0f;
    camera.width = 5.0f;
    camera.height = 5.0f;
    camera.FOV = 90.0f;
    camera.projection = ORTHOGRAPHIC_CAMERA;

    // Main loop
    bool running = true;
    while (running) {

        // Process events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        //transform.rotation.z += 0.001f;
        // Clear the screen
        ClearScreen();
        
        
        DrawSprite(sprite, transform, camera);
        // Swap buffers
        SDL_GL_SwapWindow(window.window);
    }

    // Clean up
    FreeWindowContext(window);

    return 0;
};
