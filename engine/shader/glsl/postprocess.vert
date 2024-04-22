#version 410 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_texcoord;

layout(location = 0) out vec2 texcoord;

void main()
{
    gl_Position = vec4(v_position.x, v_position.y, 0.0f, 1.0f);
    texcoord = v_texcoord;
}
