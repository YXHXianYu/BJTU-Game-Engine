#version 410 core

layout (location = 0) in float v_id;
layout (location = 1) in vec3 v_position;
layout (location = 2) in float v_face;
layout (location = 3) in float v_material_id;

layout (location = 0) out vec3 normal;
layout (location = 1) out vec2 texcoord;
layout (location = 2) out float material_id;

uniform mat4 u_model;
uniform mat4 u_view_projection;

float vertices[24][8] = vertices[24][8](
    float[8](1, 1, -1, -0, 1, -1, 0.625, 0.5),
    float[8](-1, 1, -1, -0, 1, -1, 0.875, 0.5),
    float[8](-1, 1, 1, -0, 1, 1, 0.875, 0.75),
    float[8](1, 1, 1, -0, 1, 1, 0.625, 0.75),
    float[8](1, -1, 1, -0, -0, 1, 0.375, 0.75),
    float[8](1, 1, 1, -0, -0, 1, 0.625, 0.75),
    float[8](-1, 1, 1, -0, -0, 1, 0.625, 1),
    float[8](-1, -1, 1, -0, -0, 1, 0.375, 1),
    float[8](-1, -1, 1, -1, -0, 1, 0.375, 0),
    float[8](-1, 1, 1, -1, -0, 1, 0.625, 0),
    float[8](-1, 1, -1, -1, -0, -1, 0.625, 0.25),
    float[8](-1, -1, -1, -1, -0, -1, 0.375, 0.25),
    float[8](-1, -1, -1, -0, -1, -1, 0.125, 0.5),
    float[8](1, -1, -1, -0, -1, -1, 0.375, 0.5),
    float[8](1, -1, 1, -0, -1, 1, 0.375, 0.75),
    float[8](-1, -1, 1, -0, -1, 1, 0.125, 0.75),
    float[8](1, -1, -1, 1, -0, -1, 0.375, 0.5),
    float[8](1, 1, -1, 1, -0, -1, 0.625, 0.5),
    float[8](1, 1, 1, 1, -0, 1, 0.625, 0.75),
    float[8](1, -1, 1, 1, -0, 1, 0.375, 0.75),
    float[8](-1, -1, -1, -0, -0, -1, 0.375, 0.25),
    float[8](-1, 1, -1, -0, -0, -1, 0.625, 0.25),
    float[8](1, 1, -1, -0, -0, -1, 0.625, 0.5),
    float[8](1, -1, -1, -0, -0, -1, 0.375, 0.5),
);

void main() {
    vec3 pos = vec3(vertices[v_id + face * 4.0f][0],
    vertices[v_id + face * 4.0f][1],
    vertices(v_id + face * 4.0f][2]);

    vec3 pos = pos * 0.5 + v_position;

    gl_Position =  u_view_projection * u_model * vec4(pos, 1.0);

    normal = vec3(vertices[v_id + face * 4.0f][3],
    vertices[v_id + face * 4.0f][4],
    vertices(v_id + face * 4.0f][5]);

    texcoord = vec2(vertices[v_id + face * 4.0f][6],
    vertices[v_id + face * 4.0f][7]);

    material_id = v_material_id;
}

