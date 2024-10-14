#include "./include/config.glsl"
#include "./include/utils.glsl"

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

#ifdef TRANSPARENT_SHADER
uniform float u_time;
uniform sampler2D u_noise_texture;

uniform int u_water_mode; // 0:not water; 1:static; 2:normal with noise; 3:dynamic water
#endif

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

    if (u_water_mode == 3 || u_water_mode == 4 || u_water_mode == 5) {
        if (normal.y >= 1.0 - EPS) {
            float t = u_time * 0.1;
            vec2 pos_world = (u_model * vec4(pos, 1.0)).xz;
            vec2 uv = pos_world + vec2(sin(t), cos(t));

            pos.y += -0.4 + 0.4 * texture(u_noise_texture, uv * 0.05).x;

            float x0y = 0.4 * texture(u_noise_texture, (uv + vec2(-0.1, 0.0)) * 0.05).x;
            float x1y = 0.4 * texture(u_noise_texture, (uv + vec2(0.1, 0.0)) * 0.05).x;
            float z0y = 0.4 * texture(u_noise_texture, (uv + vec2(0.0, -0.1)) * 0.05).x;
            float z1y = 0.4 * texture(u_noise_texture, (uv + vec2(0.0, 0.1)) * 0.05).x;

            float dx = x1y - x0y;
            float dy = 1.0;
            float dz = z1y - z0y;

            if (u_water_mode == 5) {
                vec2 wave_center = vec2(-5.0, 2.0);
                float wave_distance = distance(wave_center, uv);
                // TODO: the following strength is linear, should be squared
                float wave_strength = WAVE_STRENGTH * max(0.0, (WAVE_MAX_DISTANCE - wave_distance) / WAVE_MAX_DISTANCE);
                pos.y += wave_strength * sin(wave_distance * WAVE_DENSITY);
                dy += wave_strength * sin(wave_distance * WAVE_DENSITY);
                // Fail: 无法实现一个高密度的波浪，原因是曲面密度不够！可惜，以后有机会再实现吧。
            }

            float coef = -0.125;
            normal = normalize(vec3(dx * coef, dy, dz * coef)); // 这里的系数coef应该是5.0(1.0/0.2)，但效果不好，故改为经验值
        }
    }
#endif

    gl_Position =  u_view_projection * u_model * vec4(pos, 1.0);
    frag_pos = (u_model * vec4(pos, 1.0)).xyz;
    normal = (u_normal * vec4(normal, 1.0)).xyz; // normal transform
}

