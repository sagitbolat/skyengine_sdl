#version 330 core

layout (location = 0) in vec3 a_pos;
out vec3 color;

uniform float x_offset;

void main() {
    gl_Position = vec4(a_pos.x + x_offset, -a_pos.y, a_pos.z, 1.0);
    color = vec3(1.0, 1.0, 1.0);  
}