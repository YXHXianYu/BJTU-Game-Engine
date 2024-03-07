#version 410 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_texcoord;

layout (location = 0) out vec3 normal;
layout (location = 1) out vec2 texcoord;

void main() {
    gl_Position = vec4(v_position, 1.0);
    normal = v_normal;
    texcoord = v_texcoord;
}
