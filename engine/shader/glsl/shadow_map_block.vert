#version 410 core

layout (location = 0) in float v_id;
layout (location = 1) in vec3 v_position;
layout (location = 2) in float v_face;
layout (location = 3) in float v_material_id;

uniform float u_cube[192];

uniform mat4 u_light_space_matrix;
uniform mat4 u_model;

void main() {
    if (v_material_id < 0) {
        gl_Position = vec4(100.f, 100.f, 100.f, 1.f);
        return;
    }
    
    int id = int(8.0f * (v_id + v_face * 4.0f));
    vec3 pos = vec3(u_cube[id + 0], u_cube[id + 1], u_cube[id + 2]) + v_position;

    gl_Position =  u_light_space_matrix * u_model * vec4(pos, 1.0);
}