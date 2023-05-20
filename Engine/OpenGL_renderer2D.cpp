#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <gl/gl.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "image_import.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


// SECTION: Shaders.
// NOTE: Returns the shader ID
unsigned int ShaderInit(const char* vertex_path, const char* fragment_path) {
    std::string vertex_code;
    std::string fragment_code;
    std::ifstream v_shader_file;
    std::ifstream f_shader_file;
    
    v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        v_shader_file.open(vertex_path);
        f_shader_file.open(fragment_path);
        
        std::stringstream v_shader_stream, f_shader_stream;
        
        v_shader_stream << v_shader_file.rdbuf();
        f_shader_stream << f_shader_file.rdbuf();
        
        v_shader_file.close();
        f_shader_file.close();

        vertex_code = v_shader_stream.str();
        fragment_code = f_shader_stream.str();
    } catch(std::ifstream::failure const &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ " << e.what() << std::endl;
    }
    
    const char* v_shader_code = vertex_code.c_str();
    const char* f_shader_code = fragment_code.c_str();
    
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
    
    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_shader_code, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_shader_code, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };


    unsigned int ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    
    return ID;
} 

void ShaderUse(unsigned int ID) {
    glUseProgram(ID);
}

const void ShaderSetBool(unsigned int ID, const char* name, bool value) {         
    glUniform1i(glGetUniformLocation(ID, name), (int)value); 
}
const void ShaderSetInt(unsigned int ID, const char* name, int value) { 
    glUniform1i(glGetUniformLocation(ID, name), value); 
}
const void ShaderSetFloat(unsigned int ID, const char* name, float value) { 
    glUniform1f(glGetUniformLocation(ID, name), value); 
} 
const void ShaderSetTransform(unsigned int ID, const char* name, glm::mat4 trans) {
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(trans));
}


// SECTION: Rendering

struct WindowState {
    SDL_Window* window;
    SDL_GLContext* gl_context;
};

struct fColor {
    float r, g, b, a;
};

struct Vector3 {
    float x, y, z;
};


WindowState InitWindowContext(int width, int height, const char* window_name, fColor clear_color) {
    WindowState window_state = {0};


    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL: %s", SDL_GetError());
    }

    // Create an SDL window
    SDL_Window* window = SDL_CreateWindow(window_name,
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          width, height,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create SDL window: %s", SDL_GetError());
    }

    // Create an OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (glContext == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create OpenGL context: %s", SDL_GetError());
    }

    // Initialize GLEW
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize GLEW: %s", glewGetErrorString(glewError));
    }

    // Print some OpenGL information
    SDL_Log("OpenGL Version: %s", glGetString(GL_VERSION));
    SDL_Log("GLEW Version: %s", glewGetString(GLEW_VERSION));

    // Set the clear color
    glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    window_state.window = window;
    window_state.gl_context = &glContext;
    return window_state;
}

void FreeWindowContext(WindowState wdws) {

    SDL_GL_DeleteContext(*(wdws.gl_context));
    SDL_DestroyWindow(wdws.window);
    SDL_Quit();
    
}

// NOTE: Inits all the buffers and return the VAO
unsigned int InitGPUBuffers(float* vertices, size_t vertices_size, unsigned int* indices, size_t indices_size) {
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 
    // Load the texture coordinates to the VAO
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1); 
    return VAO;
}


// SECTION: Texture Loading and rendering
unsigned int LoadTexture(const char* image_path, unsigned int* width_p = nullptr, unsigned int* height_p = nullptr) {
    // NOTE: Load the image texture.
    
    //int width, height, nrChannels;
    //unsigned char *data = stbi_load(image_path, &width, &height, &nrChannels, 0);
    
    ImageData img = {0};
    LoadBitmap(image_path, &img);
    int width = img.width;
    int height = img.height;
    unsigned char* data = (unsigned char*)img.data;
    
    if (width_p != nullptr) *width_p = width;
    if (height_p != nullptr) *height_p = height;

    // NOTE: Generate a texture and bind it to current state/context
    unsigned int texture;
    glGenTextures(1, &texture); 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // NOTE: Generate the mipmap
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // NOTE: Enable mirrored repeat for both the S and T axes (the x and y axes)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // NOTE: For better scaling we use mipmapping. We set the filter to be the point filter
    // NOTE: The first one is for shrinking, so we use mipmapping for it
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    // NOTE: For magnification, we dont use mipmapping, but we can use a point filter anyway.
    // (just not a mipmap point filter.)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   
    // NOTE: Free the image afterwards:
    //stbi_image_free(data);
    FreeBitmap(&img);
    return texture; 
}


void DrawTexture(unsigned int shader_id, unsigned int texture, unsigned int VAO) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ShaderUse(shader_id);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void ClearScreen() {
    glClear(GL_COLOR_BUFFER_BIT);
}



// SECTOIN: Higher level texture loading and displaying


struct Sprite {
    unsigned int pixel_width;
    unsigned int pixel_height;
    unsigned int shader_id;
    unsigned int texture_id;
    unsigned int VAO;
};

struct Transform {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
};


enum CameraProjectionType {
    ORTHOGRAPHIC_CAMERA, 
    PERSPECTIVE_CAMERA
};
struct Camera {
    Vector3 position;
    Vector3 up_direction;
    Vector3 look_target;
    float near_plane;
    float far_plane;
    float width;
    float height;
    float FOV;

    CameraProjectionType projection;
};

// NOTE: Converts a transform object into a glm transform matrix. 
// NOTE: Rotations are done in xyz order for now.
glm::mat4 TransformToMatrix(Transform transform) {
    glm::mat4 trans = glm::mat4(1.0);

    glm::vec3 translation_vec = glm::vec3(transform.position.x, transform.position.y, transform.position.z);
    glm::vec3 rotation_vec = glm::vec3(transform.rotation.x, transform.rotation.y, transform.rotation.z);
    glm::vec3 scale_vec = glm::vec3(transform.scale.x, transform.scale.y, transform.scale.z);

    trans = glm::translate(trans, translation_vec);
    trans = glm::rotate(trans, glm::radians(rotation_vec.x), glm::vec3(1.0f, 0.0f, 0.0f));
    trans = glm::rotate(trans, glm::radians(rotation_vec.y), glm::vec3(0.0f, 1.0f, 0.0f));
    trans = glm::rotate(trans, glm::radians(rotation_vec.z), glm::vec3(0.0f, 0.0f, 1.0f));
    trans = glm::scale(trans, scale_vec); 

    return trans;
}

glm::mat4 CameraToMatrix(Camera camera) {

    // SECTION: Projection matrix
    glm::mat4 proj;
    if (camera.projection == ORTHOGRAPHIC_CAMERA) {
        proj = glm::ortho(-camera.width/2, camera.width/2, -camera.height/2, camera.height/2, camera.near_plane, camera.far_plane);
    } else if (camera.projection == PERSPECTIVE_CAMERA) {
        proj = glm::perspective(camera.FOV, camera.width/camera.height, camera.near_plane, camera.far_plane);
    }

    // SECTION: view matrix
    glm::vec3 position_vec = glm::vec3(camera.position.x, camera.position.y, camera.position.z);
    glm::vec3 target_vec = glm::vec3(camera.look_target.x, camera.look_target.y, camera.look_target.z);
    glm::vec3 up_vec = glm::vec3(camera.up_direction.x, camera.up_direction.y, camera.up_direction.z);
    
    glm::mat4 view = glm::lookAt(position_vec, target_vec, up_vec);


    glm::mat4 R = proj * view;
    return R;
}


Sprite LoadSprite(
    const char* texture_path, 
    const char* vertex_shader_path, 
    const char* fragment_shader_path
) {

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

    Sprite sprite = {0};

    sprite.shader_id =  ShaderInit(vertex_shader_path, fragment_shader_path);
    sprite.VAO = InitGPUBuffers(vertices, sizeof(vertices), indices, sizeof(indices));
    sprite.texture_id = LoadTexture(texture_path, &(sprite.pixel_width), &(sprite.pixel_height));

    return sprite;
}

void DrawSprite(Sprite sprite, Transform transform, Camera camera) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ShaderUse(sprite.shader_id);

    ShaderSetTransform(sprite.shader_id, "transform", CameraToMatrix(camera) * TransformToMatrix(transform));
    
    glBindTexture(GL_TEXTURE_2D, sprite.texture_id);
    glBindVertexArray(sprite.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}