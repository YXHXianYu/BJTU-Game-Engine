#version 410 core

layout(location = 0) out vec4 frag_color;

uniform float iTime;

void main()
{
    float b = sin(iTime) * 0.5f + 0.5f;
    frag_color = vec4(1.0f, 0.5f, b, 1.0f);
}