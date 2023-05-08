#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>

#include <stdint.h>
#include <stdio.h>

#define INT8ARRAY_TO_INT32(arr, i) (arr[i]) + (arr[i+1] << 8) + (arr[i+2] << 16) + (arr[i+3] << 24)

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource1 = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";
const char *fragmentShaderSource2 = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(0.8f, 0.8f, 0.2f, 1.0f);\n"
    "}\0";


struct ImageData{
    uint16_t width;
    uint16_t height;
    uint8_t bytes_per_pixel;
    uint8_t* data;
};

// Import BMP file
size_t ImportBMP (
    const char* image_file_name, FILE* image_file,
    uint32_t* image_width, uint32_t* image_height, 
    uint32_t* bytes_per_pixel, uint32_t* byte_offset,
    uint32_t* padding_size 
) {
    const int FILE_HEADER_SIZE = 14;
    const int INFO_HEADER_SIZE = 40;
    uint8_t BYTES_PER_PIXEL = 3;
    
    


    // SECTION: file_header
    uint8_t fh[FILE_HEADER_SIZE];
    size_t fh_size = fread(fh, sizeof(uint8_t), FILE_HEADER_SIZE, image_file);
    
    // NOTE: Error checking
    if (fh_size != FILE_HEADER_SIZE) {
        // TODO: Error
        printf("ImportBMP Error. File header sizes do not match. Possible EOF error or wrong format.%c", '\n');
    }
    if (fh[0] != 'B' && fh[1] != 'M') {
        // TODO: Filetype Error
        printf("ImportBMP Error. Incorrect filetype. Filetype must be of type bmp.%c", '\n');
    }
    // TODO: Might be unnecessary to save file size
    //uint32_t file_size = INT8ARRAY_TO_INT32(fh, 2);
    *byte_offset = INT8ARRAY_TO_INT32(fh, 10);

    
    // SECTION: info_header
    uint8_t ih[INFO_HEADER_SIZE];
    size_t ih_size = fread(ih, 1, INFO_HEADER_SIZE, image_file);
    
    *image_width   = INT8ARRAY_TO_INT32(ih, 4);
    *image_height  = INT8ARRAY_TO_INT32(ih, 8);
    uint8_t bits_per_pixel = ih[14]; 
    // NOTE: Error checking
    if (ih_size != INFO_HEADER_SIZE) {
        // TODO: Error
        printf("ImportBMP Error. Info header sizes do not match. Possible EOF error or wrong format.%c", '\n');
    }
    if (BYTES_PER_PIXEL * 8 != bits_per_pixel) {
        // TODO: Pixel Resolution Error
        BYTES_PER_PIXEL = bits_per_pixel / 8;
        printf("ImportBMP Warning. %d bits per pixel. Does not match 24. Will attempt to adjust to %d.%c", bits_per_pixel, BYTES_PER_PIXEL*8, '\n');
    }
    *bytes_per_pixel = BYTES_PER_PIXEL;

    // SECTION: image data
    uint32_t width_in_bytes = (*image_width) * BYTES_PER_PIXEL;
    *padding_size = (4 - (width_in_bytes) % 4) % 4;

    size_t image_size = (*image_width) * (*image_height) * BYTES_PER_PIXEL;
    return image_size;
}


// NOTE: Do bitmap loading.
// NOTE: Returns the address of the asset on the GameMemory->transient_storage Arena.
size_t LoadBitmap(const char* image_file_name, ImageData* image_data) {
  
    FILE* image_file = fopen(image_file_name, "rb");
    uint32_t image_width = 0;
    uint32_t image_height = 0;
    uint32_t bytes_per_pixel = 0;
    uint32_t byte_offset = 0; // NOTE: The offset in bytes from the start of the file to the image data.
    uint32_t padding_size = 0; // NOTE: The padding at the end of each line of the bmp data. 
    size_t image_size = ImportBMP(image_file_name, image_file, &image_width, &image_height, &bytes_per_pixel, &byte_offset, &padding_size);
     

    image_data->width = image_width;
    image_data->height = image_height;
    image_data->bytes_per_pixel = bytes_per_pixel;
    image_data->data = (uint8_t*)(malloc(sizeof(uint8_t) * image_size));
     

    //move to start of bitmap data
    fseek(image_file, byte_offset, SEEK_SET);
    //read the data
    for (int h = (int)image_height - 1; h >= 0; --h) {
        for (int w = 0; w < (int)image_width; ++w) {
            int i = ((h * (int)image_width) + w) * (int)bytes_per_pixel;
            for (int k = 0; k < (int)bytes_per_pixel; ++k) {
                fread(&(image_data->data[i+k]), 1, 1, image_file);
            }
        }
        if (padding_size > 0) fseek(image_file, padding_size, SEEK_CUR);         
    }
    fclose(image_file);
    
    return image_size;
}

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
    float vertices1[] = {
        0.1f,  -0.1f, 0.0f,  // center-bottom-right
        0.0f,  0.5f, 0.0f,  // top
        -0.5f, 0.0f, 0.0f,  // left
    };
    float vertices2[] = {
        -0.1f,  0.1f, 0.0f,  // center-top-left
        0.0f, -0.5f, 0.0f,  // bottom
        0.5f,  0.0f, 0.0f,  // right 
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,   // first triangle
    };  



    // NOTE: Initializing the vertex shader:
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // NOTE: Specify vertex shader sourcecode and compile it:
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // NOTE: Error-check the shader compilation:
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s", infoLog);
    }

    // NOTE: Fragment Shader:
    unsigned int fragmentShader1;
    fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader1, 1, &fragmentShaderSource1, NULL);
    glCompileShader(fragmentShader1);
    glGetShaderiv(fragmentShader1, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader1, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n %s", infoLog);
    }
    unsigned int fragmentShader2;
    fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
    glCompileShader(fragmentShader2);
    glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n %s", infoLog);
    }

    // NOTE: Create a shader program, attach the shaders to it, then link them.
    unsigned int shaderProgram1;
    shaderProgram1 = glCreateProgram();
    glAttachShader(shaderProgram1, vertexShader);
    glAttachShader(shaderProgram1, fragmentShader1);
    glLinkProgram(shaderProgram1);
    // NOTE: Error-checking the linking process
    glGetProgramiv(shaderProgram1, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram1, 512, NULL, infoLog);
        printf("ERROR::SHADER::LINKING_FAILED\n %s", infoLog);
    }
    unsigned int shaderProgram2;
    shaderProgram2 = glCreateProgram();
    glAttachShader(shaderProgram2, vertexShader);
    glAttachShader(shaderProgram2, fragmentShader2);
    glLinkProgram(shaderProgram2);
    // NOTE: Error-checking the linking process
    glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
        printf("ERROR::SHADER::LINKING_FAILED\n %s", infoLog);
    }
    // NOTE: Deallocate the shader objects now:
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader1);  
    glDeleteShader(fragmentShader2);  


    // NOTE: Create Vertex Buffer Object to send the verteces to the GPU memory
    // NOTE: The "Vertex Buffer Object" is just a raw buffer like any other buffer.
    // Its not actually a special object or struct like the VAO, which actually holds structured info
    // This buffer just happens to hold Verteces for now, but if we need to, we can store other information
    // In this buffer. The concept of the VBO is just a concept, it does not exsist concretely in the OpenGL API.
    // It is just a construct
    unsigned int VBOs[2];
    glGenBuffers(2, VBOs);
    

    // NOTE: Create an Element Buffer Object. This is just like the VBO, but for indices instead of verteces.
    // NOTE: Just like the VBO, it is also just a normal buffer, not special in any way. Calling it an EBO is just
    // a convention.
    unsigned int EBO;
    glGenBuffers(1, &EBO); 


    // NOTE: Generate and bind Vertex Array Object.
    // NOTE: Unlike the VBO and the EBO, The VAO is a real object that holds data for our shader program.
    // The data it holds is in the form of ATTRIBUTES.
    // This stores information about our VBO and EBO buffers in the form of those attributes.
    unsigned int VAOs[2];
    glGenVertexArrays(2, VAOs);    


    
    auto VAO1 = VAOs[0];
    auto VAO2 = VAOs[1];
    auto VBO1 = VBOs[0];
    auto VBO2 = VBOs[1];

    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 

    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  
    

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    






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
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shaderProgram1);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glUseProgram(shaderProgram2);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);


        // NOTE: Render in wireframe
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // NOTE: Render in filled
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Swap buffers
        SDL_GL_SwapWindow(window);
    }

    // Clean up
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
};
