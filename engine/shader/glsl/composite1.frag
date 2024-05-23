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
uniform sampler2D u_rain_texture;

uniform float u_time;
uniform vec2 u_resolution;
uniform float u_near;
uniform float u_far;
uniform vec3 u_camera_position;
uniform mat4 u_view_projection;

// === Water Reflection ===

#define WATER_STEP_BASE 0.025
#define WATER_SAMPLE_TIMES 32
#define WATER_ENABLE_JITTER true

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

vec4 water_sample_color(vec2 uv) {
    vec4 hit_color;
    hit_color.rgb = texture(u_color_texture, uv).rgb;
    float d = max(abs(uv.x - 0.5), abs(uv.y - 0.5));
    hit_color.a = sqrt1(1.0 - d * 2.0);
    return hit_color;
}

vec3 water_ray_tracing(vec3 color, vec3 start_point, vec3 direction, float fresnel) {
    vec3 test_point = start_point;
    direction *= WATER_STEP_BASE;

    bool hit = false;
    vec4 hit_color = vec4(0.0);
    vec3 test_point_in_frustum;
    vec2 uv;
    float sample_depth, test_depth;

    float jitter = 0.0f;
    if (WATER_ENABLE_JITTER) {
        vec2 uv_scaled = texcoord * u_resolution;
        jitter = mod((uv_scaled.x + uv_scaled.y) * 0.25, 1.0);
    }

    for (int i = 0; i < WATER_SAMPLE_TIMES; i++) {
        vec3 last_point = test_point;
        test_point += direction * pow(float(i + 1) + jitter, 1.46); // 使得每次采样步长增加
        test_point_in_frustum = applyViewProjectionTransform(test_point);
        uv = test_point_in_frustum.xy;
        if (uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1) {
            hit = true;
            break;
        }
        sample_depth = linearizeDepth(texture(u_depth_texture, uv).r);
        test_depth = linearizeDepth(test_point_in_frustum.z);
        
        bool magic_formula = (test_depth - sample_depth) < (1.0 / 2048.0) * (1.0 + test_depth * 200.0 + float(i)); // 去除锯齿/波纹
        if (sample_depth < test_depth && magic_formula) {
            direction = test_point - last_point;
            test_point = last_point;
            float s = 1.0;
            for(int i = 0; i < 4; i++) {
                direction *= 0.5;
                test_point += direction * s;
                test_point_in_frustum = applyViewProjectionTransform(test_point);
                uv = test_point_in_frustum.xy;
                sample_depth = linearizeDepth(texture(u_depth_texture, uv).r);
                test_depth = linearizeDepth(test_point_in_frustum.z);
                if (sample_depth < test_depth) { s = -1.0; }
                else { s = 1.0; }
            }

            hit = true;
            hit_color = water_sample_color(uv);
            break;
        }
    }
    if (!hit) {
        hit_color = water_sample_color(uv);
    }

    return mix(color, hit_color.rgb, hit_color.a * fresnel);
}

vec3 water_reflection(vec3 color, vec3 frag_pos, vec3 normal) {
    vec3 camera_to_frag = normalize(frag_pos - u_camera_position);
    vec3 reflect_dir = normalize(reflect(camera_to_frag, normal));

    float fresnel = 0.02 + 0.98 * pow(1.0 - dot(reflect_dir, normal), 2.0);

    color = water_ray_tracing(color, frag_pos + normal * 0.1, reflect_dir, fresnel);

    return color;
}

// === Main ===

void main() {
    vec2 texcoord = gl_FragCoord.xy / u_resolution;
    vec3 color = texture(u_color_texture, texcoord).rgb;
    vec3 frag_pos = texture(u_gbuffer_position, texcoord).xyz;
    vec3 normal = texture(u_gbuffer_normal, texcoord).rgb;

    // 反射效果
    float transparent_strength = texture(u_gbuffer_transparent, texcoord).a;
    if (transparent_strength > 1.0 + EPS) {
        color = color;
    } else if (transparent_strength > EPS) {
        color = water_reflection(color, frag_pos, normal);
    }

    // 添加下雨效果
    vec2 n = texture(u_rain_texture, texcoord * 0.6).rg; // 位移

    for (float r = 4.0; r > 0.0; r--) {
        vec2 grid_size = u_resolution * r * 0.005; // 网格中潜在的雨滴数量
        vec2 p = 6.28 * texcoord * grid_size + (n - 0.5) * 2.0;
        vec2 s = sin(p);

        // 当前雨滴的属性。坐标被四舍五入，以确保给定雨滴片段的一致值。
        vec4 d = texture(u_rain_texture, round(texcoord * grid_size - 0.25) / grid_size);

        // 雨滴形状和淡出
        float t = (s.x + s.y) * max(0.0, 1.0 - fract(u_time * (d.b + 0.1) + d.g) * 2.0);

        // d.r -> 只有x%的雨滴保留，x取决于雨滴的大小
        if (d.r < (5.0 - r) * 0.08 && t > 0.5) {
            vec3 v = normalize(-vec3(cos(p), mix(0.2, 2.0, t - 0.5)));
            vec2 offsetTexcoord = texcoord - v.xy * 0.3;
            vec3 dropColor = texture(u_color_texture, offsetTexcoord).rgb;

            // 混合雨滴颜色和基础颜色，增加透明度效果
            color = mix(color, dropColor, 0.5);
        }
    }

    fragcolor = vec4(color, 1.0);
}
