#version 410 core

#include "include/config.glsl"
#include "include/utils.glsl"

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform sampler2D u_color_texture;
uniform sampler2D u_depth_texture;
uniform sampler2D u_gbuffer_position;

uniform float u_time;
uniform float u_near;
uniform float u_far;
uniform vec3 u_camera_position;
uniform vec2 u_resolution;
uniform mat4 u_inverse_view;
uniform mat4 u_inverse_projection;

uniform vec3 u_sunlight_direction;

// === 3D Noise ===

// refer to: http://blog.hakugyokurou.net/?p=1630
// TODO: change to this noise https://www.shadertoy.com/view/3dVXDc
uniform sampler2D u_noise_texture;
float noise_texture(vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = smoothstep(0.0, 1.0, f);
    
    vec2 delta = vec2(37.0, 17.0) * 0.1;

    vec2 uv = (p.xy + delta * p.z) + f.xy;
    float v1 = texture(u_noise_texture, (uv) / 256.0).x;
    float v2 = texture(u_noise_texture, (uv + delta) / 256.0).x;
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

// === Cloud ===

#define CLOUD_MIN (100.0)
#define CLOUD_MAX (120.0)
#define SKY_COLOR (vec3(0.47, 0.66, 1.00))

#define CLOUD_NOISE_SPEED (vec2(5.0, 0.0))
#define CLOUD_NOISE_SCALE (0.01) // 值越小，单朵云越大，但视野内云越少
#define CLOUD_NOISE_THRESHOLD (0.4)
#define CLOUD_STEP_LENGTH (1.0)
#define CLOUD_SAMPLE_TIMES (32)

// TODO: fix z of cloud
float get_cloud_noise(vec3 cloud_pos, float cloud_min, float cloud_max) {
    float v = 1.0;
    if (cloud_pos.y < cloud_min - 1.0) {
        return 0.0;
    } else if (cloud_pos.y < cloud_min) {
        v = 1.0 - smoothstep(0.0, 1.0, cloud_min - cloud_pos.y);
    } else if (cloud_pos.y <= cloud_max) {
        v = 1.0;
    } else if (cloud_pos.y <= cloud_max + 1.0) {
        v = 1.0 - smoothstep(0.0, 1.0, min(cloud_pos.y - cloud_max, 1.0));
    } else { // cloud_pos.y > cloud_max + 1.0
        return 0.0;
    }

    cloud_pos.xy += u_time * CLOUD_NOISE_SPEED;
    cloud_pos *= CLOUD_NOISE_SCALE;

    float n = noise_hash(cloud_pos) * 0.5;
    cloud_pos *= 3.0; n += noise_hash(cloud_pos) * 0.25;
    cloud_pos *= 3.01; n += noise_hash(cloud_pos) * 0.125;
    cloud_pos *= 3.02; n += noise_hash(cloud_pos) * 0.0625;

    n *= v;

    return max(n - CLOUD_NOISE_THRESHOLD, 0.0) * (1.0 / (1.0 - CLOUD_NOISE_THRESHOLD));
}

vec4 cloud_lighting(vec4 sum, float density, float diff) {  
    vec4 color = vec4(mix(vec3(1.0,0.95,0.9), vec3(0.3,0.315,0.325), density), density);
    vec3 lighting = mix(vec3(0.7,0.75,0.8), vec3(1.8, 1.6, 1.35), diff);
    color.xyz *= lighting;
    color.a *= 0.4;
    color.rgb *= color.a;
    return sum + color * (1.0 - sum.a);
}

vec3 cloud(vec3 start_point, vec3 direction, float max_dis_2) {
    if (direction.y <= 0.1) return SKY_COLOR;
    vec3 test_point = start_point;

    float cloud_min = start_point.y + CLOUD_MIN * (exp(-start_point.y / CLOUD_MIN) + 0.001);
    float cloud_max = cloud_min + (CLOUD_MAX - CLOUD_MIN);
    float pre_step = max(0.0, (cloud_min - start_point.y) / direction.y - 0.5);
    test_point += direction * pre_step;

    float frag_distance_2 = abs2(test_point, start_point);
    if (frag_distance_2 > max_dis_2) return SKY_COLOR;

    direction *= CLOUD_STEP_LENGTH / direction.y;
    vec4 final = vec4(0.0);

    float fadeout = (1.0 - clamp(length(test_point) / 100000.0, 0.0, 1.0));

    for (int i = 0; i < CLOUD_SAMPLE_TIMES; i++) {
        // TODO: whether to add the last logical expression
        if (final.a > 0.99 || test_point.y > cloud_max) {
            break;
        }

        test_point += direction;
        float density = get_cloud_noise(test_point, cloud_min, cloud_max) * fadeout;

        if (density > 0.0) {
            float density2 = get_cloud_noise(test_point + u_sunlight_direction * 4.0, cloud_min, cloud_max);
            float diff = clamp((density - density2) * 8.0, 0.0, 1.0);
            final = cloud_lighting(final, density, diff);
        }
    }
    final = clamp(final, 0.0, 1.0);
    return SKY_COLOR * (1.0 - final.a) + final.rgb;
}

vec3 getRayDirection() { // fking difficult but charming
    vec3 ndc;
    ndc.xy = texcoord * 2.0 - 1.0;
    ndc.z = 2.0 * gl_FragCoord.z - 1.0;
    vec4 camera_space_position = u_inverse_projection * vec4(ndc, 1.0);
    camera_space_position /= camera_space_position.w;
    vec4 world_space_position = u_inverse_view * camera_space_position;
    return normalize(world_space_position.xyz - u_camera_position);
}

vec3 sky() {
    float max_dis = 1000.0;
    return cloud(u_camera_position, getRayDirection(), max_dis * max_dis);
}

void main() {
    float depth = texture(u_depth_texture, texcoord).r;
    vec3 color = texture(u_color_texture, texcoord).rgb;

    if (depth == 1.0f) {
        color = sky();
    }
    fragcolor = vec4(color, 1.0);
}
