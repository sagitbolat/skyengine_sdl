#include "Utils.cpp"

#include <stdint.h>
#include <stdio.h>

#include "shader.h"


int main(int argc, char* argv[]) {

    // NOTE: Init the SDL window and the OpenGL context.
    WindowState window = InitWindowContext(600, 600, "SkyEngine App", {0.0, 0.0, 0.0, 1.0});



    // SECTION: Drawing a triangle
    // NOTE: Specify the verticies
    float vertices[] = {
        // positions          // texture coords
        0.5f,  0.5f, 0.0f,    1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,    1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // top left 
    };

    unsigned int indices[] = { 
        0, 1, 2,   // first triangle
        0, 2, 3    // second triangle
    };  

    // NOTE: Init Shaders
    //unsigned int shader_id = ShaderInit("vertex_shader.vs", "fragment_shader.fs");
    unsigned int shader_id =  ShaderInit("../tex_shader.vs", "../tex_shader.fs");


    unsigned int VAO = InitGPUBuffers(vertices, sizeof(vertices), indices, sizeof(indices));

    unsigned int texture = LoadTexture("Test_tile.bmp");

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
        // Clear the screen
        ClearScreen();
        
        
        DrawTexture(shader_id, texture, VAO);
        // Swap buffers
        SDL_GL_SwapWindow(window.window);
    }

    // Clean up
    FreeWindowContext(window);

    return 0;
};
