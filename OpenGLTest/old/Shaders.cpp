#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include "shader.h"

int main(int argc, char* argv[]) {

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Create an SDL window
    SDL_Window* window = SDL_CreateWindow("SDL + GLEW + OpenGL Test",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          640, 480,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create SDL window: %s", SDL_GetError());
        return 1;
    }

    // Create an OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (glContext == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create OpenGL context: %s", SDL_GetError());
        return 1;
    }

    // Initialize GLEW
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize GLEW: %s", glewGetErrorString(glewError));
        return 1;
    }

    // Print some OpenGL information
    SDL_Log("OpenGL Version: %s", glGetString(GL_VERSION));
    SDL_Log("GLEW Version: %s", glewGetString(GLEW_VERSION));

    // Set the clear color
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);







    // SECTION: Drawing a triangle
    // NOTE: Specify the verticies
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f,  -0.5f, 0.0f, 
        0.0f,  0.5f,  0.0f, 
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,   // first triangle
    };  




    // NOTE: Init Shaders
    //unsigned int shader_id = ShaderInit("vertex_shader.vs", "fragment_shader.fs");
    unsigned int ID =  ShaderInit("../shader.vs", "../shader.fs");


    // NOTE: Create Vertex Buffer Object to send the verteces to the GPU memory
    // NOTE: The "Vertex Buffer Object" is just a raw buffer like any other buffer.
    // Its not actually a special object or struct like the VAO, which actually holds structured info
    // This buffer just happens to hold Verteces for now, but if we need to, we can store other information
    // In this buffer. The concept of the VBO is just a concept, it does not exsist concretely in the OpenGL API.
    // It is just a construct
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    

    // NOTE: Create an Element Buffer Object. This is just like the VBO, but for indices instead of verteces.
    // NOTE: Just like the VBO, it is also just a normal buffer, not special in any way. Calling it an EBO is just
    // a convention.
    unsigned int EBO;
    glGenBuffers(1, &EBO); 


    // NOTE: Generate and bind Vertex Array Object.
    // NOTE: Unlike the VBO and the EBO, The VAO is a real object that holds data for our shader program.
    // The data it holds is in the form of ATTRIBUTES.
    // This stores information about our VBO and EBO buffers in the form of those attributes.
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);    


    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 
    

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    






    // Main loop
    bool running = true;
    float x_offset = 0.0;
    while (running) {

        // Process events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);
        

        ShaderUse(ID);
        ShaderSetFloat(ID, "x_offset", x_offset);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);



        // NOTE: Render in point mode
        glPointSize(5.0);
        glEnable(GL_POINT_SMOOTH);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        // NOTE: Render in wireframe
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // NOTE: Render in filled
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Swap buffers
        SDL_GL_SwapWindow(window);
    }

    // Clean up
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
};
