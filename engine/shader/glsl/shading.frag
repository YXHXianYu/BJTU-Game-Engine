#version 410 core

#include "./include/config.glsl"

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform sampler2D u_gbuffer_position;
uniform sampler2D u_gbuffer_normal;
uniform sampler2D u_gbuffer_color;
uniform sampler2D u_gbuffer_transparent;
uniform sampler2D u_gbuffer_material;
uniform sampler2D u_depth_texture;

uniform float u_time;
uniform vec3 u_camera_position;

uniform int u_is_enable_depth_rendering;

// PBR Material in RTR

#define PI 3.14159265359

struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao; // ambient occlusion
};

uniform Material u_material;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 PBR_Shading(Material material, vec3 N, vec3 V, vec3 L, vec3 FragPos, vec3 lightPos, vec3 lightColor) {
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, material.albedo, material.metallic);

    vec3 H = normalize(V + L);
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lightColor * attenuation;

    float NDF = DistributionGGX(N, H, material.roughness);        
    float G = GeometrySmith(N, V, L, material.roughness);      
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - material.metallic;  

    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = (kD * material.albedo / PI + specular) * radiance * NdotL; 

    vec3 ambient = vec3(0.03) * material.albedo * material.ao;
    vec3 color = ambient + Lo;
    
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 
    return color;
}

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

// === Main ===

vec3 shading() {
    vec3  frag_pos = texture(u_gbuffer_position, texcoord).rgb;
    vec3  normal   = texture(u_gbuffer_normal, texcoord).rgb;
    vec3  kd       = texture(u_gbuffer_color, texcoord).rgb;
    float ks       = texture(u_gbuffer_color, texcoord).a;
    vec3  mat      = texture(u_gbuffer_material, texcoord).rgb;
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

    vec3 N = normal;
    vec3 V = normalize(u_camera_position - frag_pos);
    vec3 color = vec3(0.0);
    Material material = Material(kd, mat.r, mat.g, mat.b);

    for (int i = 0; i < u_dirlights_cnt; i++) {
        vec3 L = -u_dirlights[i].dir;
        color += PBR_Shading(material, N, V, L, frag_pos, frag_pos - u_dirlights[i].dir * 1.0, u_dirlights[i].color);
    }
    for (int i = 0; i < u_spotlights_cnt; i++) {
        vec3 L = normalize(u_spotlights[i].pos - frag_pos);
        // color += PBR_Shading(material, N, V, L, frag_pos, u_spotlights[i].pos, u_spotlights[i].color);
    }

    float shadow = calc_shadow(u_light_space_matrix * vec4(frag_pos, 1.0));

    return ambient + (1.0 - shadow) * (color);
    // return (1.0 - shadow) * color;
}

void main() {
    fragcolor = vec4(shading(), 1.0);
}
