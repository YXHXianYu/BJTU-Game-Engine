#version 410 core

#include "./include/config.glsl"
#include "./include/utils.glsl"

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform sampler2D u_gbuffer_position;
uniform sampler2D u_gbuffer_normal;
uniform sampler2D u_gbuffer_color;
uniform sampler2D u_gbuffer_transparent;
uniform sampler2D u_depth_texture;

uniform float u_time;
uniform vec3 u_camera_position;

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
        vec3 cam_dir = normalize(u_camera_position - frag_pos);
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

// === Reflective Shadow Map ===
uniform int u_is_enable_rsm;
uniform int u_rsm_width;
uniform int u_rsm_height;
uniform sampler2D u_rsm_position_texture;
uniform sampler2D u_rsm_normal_texture;
uniform sampler2D u_rsm_color_texture;

#define RSM_STRENGTH 0.15
#define RSM_ANGLE_THRESHOLD 0.01
#define RSM_DISTANCE_ATTENUATION 1.25

// === Main ===

#define ATTENUATION_THRESHOLD 0.2

vec3 shading() {
    vec3  frag_pos = texture(u_gbuffer_position, texcoord).rgb;
    vec3  normal   = texture(u_gbuffer_normal, texcoord).rgb;
    vec3  kd       = texture(u_gbuffer_color, texcoord).rgb;
    float ks       = texture(u_gbuffer_color, texcoord).a;
    float depth    = texture(u_depth_texture, texcoord).r;
    if (depth == 1.0) {
        return vec3(0.0);
    }

    vec3  transparent_color = texture(u_gbuffer_transparent, texcoord).rgb;
    float transparent_alpha = texture(u_gbuffer_transparent, texcoord).a;
    if (transparent_alpha > 1.0 + EPS) {
        transparent_alpha -= 1.0;
        kd = mix(kd, transparent_color, transparent_alpha);
    } else if (transparent_alpha > EPS) {
        kd = mix(kd, transparent_color, transparent_alpha);
        ks = min(1.0, ks * 1.75); // magic formula, make sun light reflected by water stronger
    }

    vec3 ambient  = AMBIENT_STRENGTH * kd * u_dirlights[0].color; // u_dirlights[0] is the sun
    vec3 diffuse  = vec3(0.0);
    vec3 specular = vec3(0.0);

    // directional lights
    for (int i = 0; i < u_dirlights_cnt; i++) {
        vec3 diffuse_light, specular_light;

        calc_light(frag_pos, normal, -u_dirlights[i].dir, u_dirlights[i].color, diffuse_light, specular_light);

        diffuse += diffuse_light;
        specular += specular_light;
    }
    // spot lights
    for (int i = 0; i < u_spotlights_cnt; i++) {
        vec3 diffuse_light, specular_light;

        vec3 light_dir = normalize(u_spotlights[i].pos - frag_pos);
        float dis = length(u_spotlights[i].pos - frag_pos);
        float attenuation = 1.0 / (1 + 0.09 * dis + 0.032 * (dis * dis)); 
        
        if (attenuation <= ATTENUATION_THRESHOLD) continue;

        calc_light(frag_pos, normal, light_dir, u_spotlights[i].color * attenuation, diffuse_light, specular_light);

        diffuse += diffuse_light;
        specular += specular_light;
    }

    // RSM
    if (u_is_enable_rsm >= 1) {
        vec3 rsm_ambient = vec3(0.0);

        vec3 diffuse_light, specular_light;
        vec3 light_dir = -u_dirlights[0].dir;
        vec3 light_color = u_dirlights[0].color;

        float di = 1.0 / float(u_rsm_width);
        float dj = 1.0 / float(u_rsm_height);

        for (float i = 0.0; i < 1.0; i += di) {
           for (float j = 0.0; j < 1.0; j += dj) {
                vec3 rsm_position = texture(u_rsm_position_texture, vec2(i, j)).rgb;
                float dis2 = abs2(rsm_position - frag_pos);
                float attenuation = 1.0 / (1 + RSM_DISTANCE_ATTENUATION * dis2); 
                if (attenuation < ATTENUATION_THRESHOLD) continue;

                vec3 rsm_normal = texture(u_rsm_normal_texture, vec2(i, j)).rgb;
                if (dot(rsm_normal, normal) >= RSM_ANGLE_THRESHOLD) continue;

                vec3 rsm_light_dir = normalize(rsm_position - frag_pos);
                if (dot(rsm_normal, -rsm_light_dir) <= RSM_ANGLE_THRESHOLD) continue;
                if (dot(normal, rsm_light_dir) <= RSM_ANGLE_THRESHOLD) continue;

                vec4 rsm_color_raw = texture(u_rsm_color_texture, vec2(i, j)).rgba;

                vec3 rsm_color = light_color * rsm_color_raw.rgb * attenuation;
                rsm_color *= max(dot(rsm_normal, light_dir), 0.0); // 1-st time reflect

                calc_light(frag_pos, normal, rsm_light_dir, rsm_color, diffuse_light, specular_light); 

                rsm_ambient += diffuse_light;
            }
        }

        ambient = ambient + rsm_ambient * RSM_STRENGTH;
    }

    float shadow = calc_shadow(u_light_space_matrix * vec4(frag_pos, 1.0));

    return ambient + (1.0 - shadow) * (kd * diffuse * DIFFUSE_STRENGTH + ks * specular);
}

void main() {
    fragcolor = vec4(shading(), 1.0);
}
