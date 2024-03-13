#version 410 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_texcoord;

layout (location = 0) out vec3 normal;
layout (location = 1) out vec2 texcoord;
layout (location = 2) out vec3 FragPos;

uniform mat4 u_model;
uniform mat4 u_view_projection;

void main() {
    vec3 pos = v_position;

    gl_Position =  u_view_projection * u_model * vec4(pos, 1.0);

    FragPos = vec3(u_model * vec4(pos, 1.0));

    normal = v_normal;
    texcoord = v_texcoord;
}