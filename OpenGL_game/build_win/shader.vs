#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_texture_coord;
out vec2 tex_coord;
out vec4 color_multiplier;
uniform mat4 transform;
uniform mat4 view;
uniform vec4 i_color_multiplier;
void main()
{
gl_Position = transform * vec4(a_pos.x, a_pos.y, a_pos.z, 1.0);
tex_coord = a_texture_coord;
color_multiplier = i_color_multiplier;
}
