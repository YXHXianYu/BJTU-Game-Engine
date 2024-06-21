
#include "./include/config.glsl"

layout (location = 0) in vec3 normal;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 frag_pos;

layout (location = 0) out vec3 gbuffer_position;
layout (location = 1) out vec3 gbuffer_normal;
layout (location = 2) out vec4 gbuffer_color;
layout (location = 3) out vec4 gbuffer_transparent;
layout (location = 4) out vec4 gbuffer_material;

#ifdef BLOCK_SHADER
layout (location = 3) flat in int material_id;

uniform sampler3D u_block_texture;
#endif

#ifdef MODEL_SHADER
uniform int u_texture_cnt;
uniform sampler2D u_texture_diffuse;
// uniform sampler2D u_texture_specular;
uniform vec3 u_diffuse_color;

struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};
uniform Material u_material;
#endif

#ifdef TRANSPARENT_SHADER
uniform float u_time;
uniform vec3 u_camera_position;
uniform vec4 u_transparent_info; // rgba: color + alpha

uniform int u_water_mode; // 0:not water; 1:static; 2:normal with noise; 3:dynamic water

// refer to: https://www.shadertoy.com/view/ldlXzM
// hash based 3d value noise
float hash(float n) {
    return fract(sin(n)*43758.5453);
}
float noise_hash(in vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0 + 113.0*p.z;
    return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                   mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
               mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                   mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}
#endif

void main() {

    gbuffer_position = frag_pos;
    gbuffer_normal = normalize(normal); // TODO: check if it's needed

#ifdef BLOCK_SHADER
    gbuffer_color.rgb = texture(u_block_texture, vec3(texcoord, float(material_id) / float(MATERIAL_ID_SUM - 1))).rgb;
    gbuffer_color.a = 0.4; // TODO: customized specular

    // TODO: according to material_id, set metallic, roughness, ao (for drj)
    if (material_id == 1) { // stone
        gbuffer_material.rgb = vec3(0.8, 0.2, 0.5);
    } else if (material_id == 2) { // grass
        gbuffer_material.rgb = vec3(1.0, 0.9, 0.5);
    } else if (material_id == 3) { // dirt
        gbuffer_material.rgb = vec3(0.0, 0.8, 1.0);
    } else if (material_id == 4) { // rock
        gbuffer_material.rgb = vec3(0.6, 0.6, 0.8);
    } else if (material_id == 5) { // wood planks
        gbuffer_material.rgb = vec3(0.0, 0.5, 1.0);
    } else if (material_id == 6) { // wood logs
        gbuffer_material.rgb = vec3(0.0, 0.7, 1.0);
    } else {
        gbuffer_material.rgb = vec3(0.0, 0.5, 1.0);
    }
#endif

#ifdef MODEL_SHADER
    if (u_texture_cnt == 0) {
        gbuffer_color.rgb = clamp(u_material.albedo.rgb, 0.0, 1.0);
    } else {
        gbuffer_color.rgb = texture(u_texture_diffuse, texcoord).rgb;
    }
    gbuffer_color.a = 0.4; // TODO: customized specular

    gbuffer_material.r = u_material.metallic;
    gbuffer_material.g = u_material.roughness;
    gbuffer_material.b = u_material.ao;
#endif

#ifdef TRANSPARENT_SHADER
    gbuffer_transparent = u_transparent_info;
    if (dot(normal, frag_pos - u_camera_position) >= 0) {
        gbuffer_transparent.a = 1.0 + u_transparent_info.a; // transparent but no reflection
    }

    if (u_water_mode == 2 || u_water_mode == 4) {
        float freq = 40.0f;
        float coef = 0.025f;
        float t = u_time * 0.0;
        float dx = noise_hash(vec3(frag_pos.xz * freq, sin(t)));
        float dz = noise_hash(vec3(frag_pos.xz * freq, cos(t)));
        gbuffer_normal = normalize(vec3((dx - 0.5) * coef + gbuffer_normal.x, gbuffer_normal.y, (dz - 0.5) * coef + gbuffer_normal.z));
    }
#endif
}
