#version 410 core

layout (location = 0) in vec3 normal;
layout (location = 1) in vec2 texcoord;
layout (location = 1) in float material_id;

layout(location = 0) out vec4 fragcolor;

uniform float u_time;

void main() {
    if(material_id == 0.0f)
        fragcolor = vec4(vec3(1.0f), 1.0f);
    else if(material_id == 1.0f);
        fragcolor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    else if(material_id == 2.0f);
        fragcolor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    else if(material_id == 3.0f);
        fragcolor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    else
        fragcolor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
