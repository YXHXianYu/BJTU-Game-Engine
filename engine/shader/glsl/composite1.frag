#version 410 core

#include "include/config.glsl"
#include "include/utils.glsl"

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform sampler2D u_color_texture;
uniform sampler2D u_depth_texture;
uniform sampler2D u_gbuffer_position;
uniform sampler2D u_gbuffer_normal;
uniform sampler2D u_gbuffer_transparent;

uniform float u_near;
uniform float u_far;
uniform vec3 u_camera_position;
uniform mat4 u_view_projection;

// === Water Reflection ===

#define WATER_STEP_BASE 0.05
#define WATER_SAMPLE_TIMES 256

vec3 applyViewProjectionTransform(vec3 position) {
    vec4 p = vec4(position, 1.0);
    p = u_view_projection * p;
    p /= p.w;
    if (p.z < -1 || p.z > 1) return vec3(-1.0);
    p = p * 0.5f + 0.5f;
    return p.xyz;
}

float linearizeDepth(float depth) {
    return (2.0 * u_near) / (u_far + u_near - depth * (u_far - u_near));
}

vec3 water_ray_tracing(vec3 color, vec3 start_point, vec3 direction) {
    vec3 test_point = start_point;
    direction *= WATER_STEP_BASE;

    bool hit = false;
    vec4 hit_color = vec4(0.0);
    vec3 test_point_in_frustum;
    float sample_depth, test_depth;

    for (int i = 0; i < WATER_SAMPLE_TIMES; i++) {
        test_point += direction * pow(float(i + 1), 1.46); // 使得每次采样步长增加
        test_point_in_frustum = applyViewProjectionTransform(test_point);
        vec2 uv = test_point_in_frustum.xy;
        if (uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1) {
            hit = true;
            break;
        }
        sample_depth = linearizeDepth(texture(u_depth_texture, uv).r);
        test_depth = linearizeDepth(test_point_in_frustum.z);
        
        bool magic_formula = test_depth - sample_depth < (1.0 / 2048.0) * (1.0 + test_depth * 200.0 + float(i)); // 去除锯齿/波纹
        if (sample_depth < test_depth && magic_formula) {
            // TODO: bisearch fix
            hit = true;
            hit_color = vec4(texture(u_color_texture, uv).rgb, 1.0);
            break;
        }
    }
    if (!hit && sample_depth >= 1.0 - EPS) {
        hit_color = vec4(vec3(1.0), 1.0);
        // hit_color = vec4(texture(u_color_texture, test_point_in_frustum.xy).rgb, 1.0);
    }

    return hit_color.rgb;
}

vec3 water_reflection(vec3 color, vec3 frag_pos, vec3 normal) {
    vec3 camera_to_frag = normalize(frag_pos - u_camera_position);
    vec3 reflect_dir = normalize(reflect(camera_to_frag, normal));

    color = water_ray_tracing(color, frag_pos + normal * 0.1, reflect_dir);

    return color;
}

// === Main ===

void main() {
    vec3 color    = texture(u_color_texture, texcoord).rgb;
    vec3 frag_pos = texture(u_gbuffer_position, texcoord).xyz;
    vec3 normal   = texture(u_gbuffer_normal, texcoord).rgb;

    if (texture(u_gbuffer_transparent, texcoord).a > EPS) {
        color = water_reflection(color, frag_pos, normal);
    }

    fragcolor = vec4(color, 1.0);
}
