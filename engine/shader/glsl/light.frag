#version 410 core

layout (location = 0) in vec3 normal;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 frag_pos;

layout (location = 0) out vec4 fragcolor;

uniform float u_time;
uniform vec3 u_light_color;

void main() {
    vec3 pos = frag_pos;
    fragcolor = vec4(u_light_color, 1.0f);
}
