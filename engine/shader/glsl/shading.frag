#version 410 core

#include "./include/config.glsl"

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform sampler2D u_gbuffer_position;
uniform sampler2D u_gbuffer_normal;
uniform sampler2D u_gbuffer_color;
uniform sampler2D u_depth_texture;

uniform float u_time;
uniform vec2 u_resolution;
uniform vec3 u_camera_pos;

uniform int u_is_enable_depth_rendering;

// === Lights ===

struct SpotLight {
    vec3 pos;
    vec3 color;
};

struct DirLight {
    vec3 dir; // we have normalized this vector in render_directed_light.h
    vec3 color;
};

uniform int u_spotlights_cnt;
uniform SpotLight u_spotlights[10];

uniform int u_dirlights_cnt;
uniform DirLight u_dirlights[10];

void calc_light(
    vec3 frag_pos, vec3 normal, vec3 light_dir, vec3 light_color,
    out vec3 diffuse, out vec3 specular
) {
    diffuse = max(dot(normal, light_dir), 0.0) * light_color;

    // light_dir = normalize(light_dir);
    if (dot(light_dir, normal) > 0.0) {
        vec3 cam_dir = normalize(u_camera_pos - frag_pos);
        vec3 reflect_dir = reflect(-light_dir, normal);
        specular = pow(max(dot(cam_dir, reflect_dir), 0.0), 64) * light_color;
    } else {
        specular = vec3(0.0);
    }
}

// === Shadow Map ===

uniform int u_is_enable_shadow_map;
uniform sampler2D u_shadow_texture;
uniform mat4 u_light_space_matrix;

float calc_shadow(vec4 frag_pos_light_space) {
    vec3 projection_pos = frag_pos_light_space.xyz / frag_pos_light_space.w;
    projection_pos = projection_pos * 0.5 + 0.5;

    if (projection_pos.z > 1.0) { return 0.0; }

    float depth_in_shadow_map = texture(u_shadow_texture, projection_pos.xy).r;
    float depth_in_frag = projection_pos.z;
    float shadow = ((depth_in_frag > depth_in_shadow_map + SHADOW_MAP_EPS) ? 1.0 : 0.0);
    return shadow;
}

// === Main ===

vec3 shading() {
    vec3  frag_pos = texture(u_gbuffer_position, texcoord).rgb;
    vec3  normal   = texture(u_gbuffer_normal, texcoord).rgb;
    vec3  kd       = texture(u_gbuffer_color, texcoord).rgb;
    float ks       = texture(u_gbuffer_color, texcoord).a;
    float depth    = texture(u_depth_texture, texcoord).r;
    if (depth == 1.0) {
        return vec3(0.0);
    }

    vec3 ambient  = AMBIENT_STRENGTH * kd * u_dirlights[0].color; // u_dirlights[0] is the sun
    vec3 diffuse  = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (int i = 0; i < u_dirlights_cnt; i++) {
        vec3 diffuse_light, specular_light;

        calc_light(frag_pos, normal, -u_dirlights[i].dir, u_dirlights[i].color, diffuse_light, specular_light);

        diffuse += diffuse_light;
        specular += specular_light;
    }
    for (int i = 0; i < u_spotlights_cnt; i++) {
        vec3 diffuse_light, specular_light;

        vec3 light_dir = normalize(u_spotlights[i].pos - frag_pos);
        float dis = length(u_spotlights[i].pos - frag_pos);
        float attenuation = 1.0 / (1 + 0.09 * dis + 0.032 * (dis * dis)); 
        calc_light(frag_pos, normal, light_dir, u_spotlights[i].color * attenuation, diffuse_light, specular_light);

        diffuse += diffuse_light;
        specular += specular_light;
    }

    float shadow = calc_shadow(u_light_space_matrix * vec4(frag_pos, 1.0));

    return ambient + (1.0 - shadow) * (kd * diffuse * DIFFUSE_STRENGTH + ks * specular);
}

void main() {
    fragcolor = vec4(shading(), 1.0);
}