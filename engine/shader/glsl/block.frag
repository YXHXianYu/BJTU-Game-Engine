#version 410 core

#include "./glsl/material_id.glsl"

layout (location = 0) in vec3 normal;
layout (location = 1) in vec2 texcoord;
layout (location = 2) flat in int material_id;

layout (location = 0) out vec4 fragcolor;

uniform float u_time;
uniform sampler3D u_texture;

void main() {
    fragcolor = texture(u_texture, vec3(texcoord, float(material_id) / float(MATERIAL_ID_SUM - 1)));
}
