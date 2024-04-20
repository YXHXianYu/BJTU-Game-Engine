#version 410 core

layout (location = 0) in vec3 v_position;
// layout (location = 1) in vec3 v_normal;
// layout (location = 2) in vec2 v_texcoord;

uniform mat4 u_light_space_matrix;
uniform mat4 u_model;

void main() {
    gl_Position = u_light_space_matrix * u_model * vec4(v_position, 1.0);
}