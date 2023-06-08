#pragma once

#include "sky_structs.h"




// SECTION: Shaders.
// NOTE: Returns the shader ID. Calling with no arguments will use the default shaders.
GL_ID* ShaderInit(const char* vertex_path = nullptr, const char* fragment_path = nullptr);
void FreeShaders(GL_ID* shader);
void ShaderUse(GL_ID* shader);
void ShaderSetBool(GL_ID* shader, const char* name, bool value);
void ShaderSetInt(GL_ID* shader, const char* name, int value);
void ShaderSetFloat(GL_ID* shader, const char* name, float value);

// SECTION: Rendering
WindowContext* InitWindowContext(int width, int height, const char* window_name, fColor clear_color);
void FreeWindowContext(WindowContext* window_context);
// NOTE: Returns a handler to the gpu buffers (VBO, VAO, EBO). If called with no arguments, will init to default buffers.
GPUBufferIDs InitGPUBuffers(float* vertices = nullptr, size_t vertices_size = 0, unsigned int* indices = nullptr, size_t indices_size = 0);
void FreeGPUBuffers(GPUBufferIDs);
void ClearScreen();


// SECTION: Higher Level Loading and Displaying
Sprite LoadSprite(
    const char* texture_path, 
    GL_ID* shader_id, GPUBufferIDs buffers 
);
void FreeSprite(Sprite);
void DrawSprite(Sprite sprite, Transform transform, Camera camera);


// NOTE: For engine use

unsigned int SkyGetGLID(GL_ID* id);