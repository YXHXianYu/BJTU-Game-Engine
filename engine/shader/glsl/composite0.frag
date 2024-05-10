#version 410 core

#include "include/config.glsl"
#include "include/utils.glsl"

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform sampler2D u_color_texture;
uniform sampler2D u_depth_texture;
uniform sampler2D u_gbuffer_position;
uniform sampler2D u_gbuffer_normal;

uniform float u_time;
uniform float u_near;
uniform float u_far;
uniform vec3 u_camera_position;
uniform vec2 u_resolution;
uniform mat4 u_inverse_view;
uniform mat4 u_inverse_projection;

uniform vec3 u_sunlight_direction;

// === Utils ===

vec3 getRayDirection() { // fking difficult but charming
    vec3 ndc;
    ndc.xy = texcoord * 2.0 - 1.0;
    ndc.z = 2.0 * gl_FragCoord.z - 1.0;
    vec4 camera_space_position = u_inverse_projection * vec4(ndc, 1.0);
    camera_space_position /= camera_space_position.w;
    vec4 world_space_position = u_inverse_view * camera_space_position;
    return normalize(world_space_position.xyz - u_camera_position);
}

// === 3D Noise ===

// refer to: http://blog.hakugyokurou.net/?p=1630
// TODO: change to this noise https://www.shadertoy.com/view/3dVXDc
uniform sampler2D u_noise_texture;
float noise_texture(vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = smoothstep(0.0, 1.0, f);

    vec2 uv = (p.xy + vec2(37.0, 17.0) * p.z) + f.xy;
    float v1 = texture(u_noise_texture, (uv) * 0.004).x; // *0.004 = /256.0
    float v2 = texture(u_noise_texture, (uv + vec2(37.0, 17.0)) * 0.004).x;
    return mix(v1, v2, f.z);
}

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

// === Cloud & Fog ===

#define CLOUD_MIN   (40.0)
#define SKY_COLOR (vec3(0.47, 0.66, 1.00))
#define FOG_COLOR (SKY_COLOR + vec3(0.1))

#define CLOUD_NOISE_SPEED (vec3(5.0, 0.0, 0.0))
#define CLOUD_NOISE_SCALE (0.02) // 值越小，单朵云越大，但视野内云越少
#define CLOUD_SAMPLE_TIMES (64)

#define NOISE noise_texture
float get_cloud_noise(vec3 p) {
    vec3 q = p - CLOUD_NOISE_SPEED * u_time;
    q *= CLOUD_NOISE_SCALE;
    float f;
    f  = 0.50000 * NOISE(q); q = q*2.02;
    f += 0.25000 * NOISE(q); q = q*2.03;
    f += 0.12500 * NOISE(q);
    f = clamp(1.0 * f + min(p.y * 0.025, 0.5) - 0.5, 0.0, 1.0); // 让云的下部更薄 (*0.025 = /40.0)
    f = max(f - 0.5, 0.0) * 2.0; // threshold
    // TODO: add a smoothstep
    return f;
}

// refer to iq's Clouds: https://www.shadertoy.com/view/XslGRr
vec3 cloud(vec3 base_color, vec3 start_point, vec3 direction) {
    if (direction.y <= 0.1) return SKY_COLOR;

    vec4 sum = vec4(0.0);
    float cloud_min = start_point.y + CLOUD_MIN * (exp(-start_point.y / CLOUD_MIN) + 0.001);
    float t = 0.05 + ((cloud_min - start_point.y) / direction.y);

    for (int i = 0; i < CLOUD_SAMPLE_TIMES; i++) {
        if (sum.a > 0.99) break;

        vec3 pos = start_point + direction * t;
        float density = get_cloud_noise(vec3(pos.x, pos.y - cloud_min + CLOUD_MIN, pos.z));
        if (density > 0.01) {
            pos = pos + u_sunlight_direction * 0.3;
            float density2 = get_cloud_noise(vec3(pos.x, pos.y - cloud_min + CLOUD_MIN, pos.z));

            float diff = clamp((density - density2) * 1.67, 0.0, 1.0); // *1.67 = /0.6
            vec3 lighting = vec3(1.0, 0.6, 0.3) * diff + vec3(0.91, 0.98, 1.05);
            vec4 color = vec4(mix(vec3(1.0, 0.95, 0.8), vec3(0.25, 0.3, 0.35), density), density);
            color.rgb *= lighting;
            color.w *= 0.4;
            color.rgb *= color.w;
            sum += color * (1.0 - sum.w);
        }
        t += 1.0;
    }

    sum = clamp(sum, 0.0, 1.0);
    return base_color * (1.0 - sum.a) + sum.rgb;
}

// designed & created by me, YXHXianYu
vec4 fog_sky(vec3 direction) {
    if (direction.y <= -0.2 || direction.y >= 0.4) return vec4(0.0);

    if (direction.y >= 0.1) {
        return vec4(FOG_COLOR, smoothstep(0.0, 1.0, (0.4 - direction.y) * 3.33)); // *3.33 = /0.3
    } else {
        return vec4(FOG_COLOR, 1.0);
    }
}

vec4 fog_frag(vec3 direction, float dis2) {
    if (dis2 <= 900.0 || direction.y <= -0.2 || direction.y >= 0.4) return vec4(0.0);

    return vec4(FOG_COLOR, min(sqrt1((dis2 - 900.0) * 0.00143), 1.0)); // *0.00143 = /700
}

// === Sun ===

#define SUN_COLOR (vec3(1.0, 0.95, 0.8))
#define SUN_COEF_SIZE 32
#define SUN_COEF_MIN  0.1
#define SUN_COEF_MAX  0.7

vec3 sun(vec3 direction) {
    float v = pow(max(-dot(direction, u_sunlight_direction), 0.0), SUN_COEF_SIZE);
    if (v <= SUN_COEF_MIN) {
        return SKY_COLOR;
    } else if (v <= SUN_COEF_MAX) {
        return mix(SKY_COLOR, SUN_COLOR, (v - SUN_COEF_MIN) / (SUN_COEF_MAX - SUN_COEF_MIN));
    } else {
        return SUN_COLOR;
    }
}

// === Main ===

void main() {
    float depth   = texture(u_depth_texture, texcoord).r;
    vec3 color    = texture(u_color_texture, texcoord).rgb;
    vec3 frag_pos = texture(u_gbuffer_position, texcoord).xyz;
    vec3 normal   = texture(u_gbuffer_normal, texcoord).rgb;
    vec3 ray_direction = getRayDirection();

    if (depth == 1.0f) { // sky
        color = sun(ray_direction);
        color = cloud(color, u_camera_position, ray_direction);
        vec4 f = fog_sky(ray_direction);
        color = mix(color, f.rgb, f.a);
    } else { // frag
        float dis2 = abs2(frag_pos - u_camera_position);
        vec4 f = fog_frag(ray_direction, dis2);
        color = mix(color, f.rgb, f.a);
    }

    fragcolor = vec4(color, 1.0);
}
