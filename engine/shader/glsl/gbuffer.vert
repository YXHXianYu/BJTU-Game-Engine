// Input

#ifdef BLOCK_SHADER
layout (location = 0) in float v_id;
layout (location = 1) in vec3 v_position;
layout (location = 2) in float v_face;
layout (location = 3) in float v_material_id;
#endif

#ifdef MODEL_SHADER
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_texcoord;
#endif

#ifdef TRANSPARENT_SHADER
layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_texcoord;
#endif

// Ouput

layout (location = 0) out vec3 normal;
layout (location = 1) out vec2 texcoord;
layout (location = 2) out vec3 frag_pos;

#ifdef BLOCK_SHADER
uniform float u_cube[192];
layout (location = 3) flat out int material_id;
#endif

uniform mat4 u_model;
uniform mat4 u_normal;
uniform mat4 u_view_projection;

void main() {

#ifdef BLOCK_SHADER
    if (v_material_id < 0) {
        gl_Position = vec4(100.f, 100.f, 100.f, 1.f);
        normal = vec3(0.0f);
        texcoord = vec2(0.0f);
        material_id = int(v_material_id);
        frag_pos = vec3(100.f);
        return;
    }
    
    int id = int(8.0f * (v_id + v_face * 4.0f));
    vec3 pos = vec3(u_cube[id + 0], u_cube[id + 1], u_cube[id + 2]) + v_position;

    normal = vec3(u_cube[id + 3], u_cube[id + 4], u_cube[id + 5]);
    texcoord = vec2(u_cube[id + 6], u_cube[id + 7]);
    material_id = int(v_material_id);
#endif

#ifdef MODEL_SHADER
    vec3 pos = v_position;

    normal = v_normal;
    texcoord = v_texcoord;
#endif

#ifdef TRANSPARENT_SHADER
    vec3 pos = v_position;

    normal = v_normal;
    texcoord = v_texcoord;
#endif

    gl_Position =  u_view_projection * u_model * vec4(pos, 1.0);
    frag_pos = (u_model * vec4(pos, 1.0)).xyz;
    normal = (u_normal * vec4(normal, 1.0)).xyz; // normal transform
}

