#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_texture_coord;
layout (location = 2) in vec3 a_offset;
uniform mat4 transform;
out vec2 tex_coord;
void main()
{
    gl_Position = transform * vec4(a_pos.x + a_offset.x, a_pos.y + a_offset.y, a_pos.z + a_offset.z, 1.0);
    tex_coord = a_texture_coord;
}