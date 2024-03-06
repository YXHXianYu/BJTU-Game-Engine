#version 410 core

layout (location = 0) in vec3 normal;
layout (location = 1) in vec2 tex_coord;

layout(location = 0) out vec4 frag_color;

uniform float iTime;

void main() {
    float b = sin(iTime * 2.0f) * 0.5f + 0.5f;
    frag_color = vec4(tex_coord.x, tex_coord.y, b, 1.0f);
}