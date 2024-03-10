#version 410 core

layout (location = 0) in vec3 normal;
layout (location = 1) in vec2 texcoord;

layout(location = 0) out vec4 fragcolor;

uniform float u_time;
uniform vec3 lightColor;

void main() {
    fragcolor = vec4(lightColor, 1.0f);
}