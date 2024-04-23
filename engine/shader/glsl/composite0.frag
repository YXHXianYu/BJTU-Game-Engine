#version 410 core

#include "include/config.glsl"
#include "include/utils.glsl"

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform sampler2D u_color_texture;
uniform sampler2D u_depth_texture;
uniform sampler2D u_gbuffer_position;

uniform float u_time;
uniform vec3 u_camera_position;
uniform vec2 u_resolution;
uniform mat4 u_inverse_view;
uniform mat4 u_inverse_projection;

// === 3D Noise ===

// refer to: http://blog.hakugyokurou.net/?p=1630
// TODO: change to this noise https://www.shadertoy.com/view/3dVXDc
uniform sampler2D u_noise_texture;
float noise(vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = smoothstep(0.0, 1.0, f);
    
    vec2 delta = vec2(37.0, 17.0) * 0.1;

    vec2 uv = (p.xy + delta * p.z) + f.xy;
    float v1 = texture(u_noise_texture, (uv) / 256.0).x;
    float v2 = texture(u_noise_texture, (uv + delta) / 256.0).x;
    return mix(v1, v2, f.z);
}

// === Cloud ===

#define CLOUD_MIN (50.0)
#define CLOUD_MAX (70.0)
#define SKY_COLOR (vec3(0.47, 0.66, 1.00))

#define CLOUD_NOISE_SPEED (vec2(5.0, 0.0))
#define CLOUD_NOISE_SCALE (0.02) // 值越小，单朵云越大，但视野内云越少
#define CLOUD_NOISE_THRESHOLD (0.60)

// TODO: fix z of cloud
float get_cloud_noise(vec3 cloud_pos) {
    cloud_pos.xy += u_time * CLOUD_NOISE_SPEED;
    cloud_pos *= CLOUD_NOISE_SCALE;

    float n = noise(cloud_pos) * 0.5;
    cloud_pos *= 3.0; n += noise(cloud_pos) * 0.25;
    cloud_pos *= 3.01; n += noise(cloud_pos) * 0.125;
    cloud_pos *= 3.02; n += noise(cloud_pos) * 0.0625;

    return max(n - CLOUD_NOISE_THRESHOLD, 0.0) * (1.0 / (1.0 - CLOUD_NOISE_THRESHOLD));
}

vec3 cloud(vec3 start_point, vec3 direction, float max_dis_2) {
    if (direction.y <= 0.1) return SKY_COLOR;
    vec3 test_point = start_point;

    float cloud_min = start_point.y + CLOUD_MIN * (exp(-start_point.y / CLOUD_MIN) + 0.001);
    float cloud_max = cloud_min + (CLOUD_MAX - CLOUD_MIN);
    float distance_to_cloud = cloud_min - start_point.y;
    float pre_step = distance_to_cloud / direction.y;
    test_point += direction * pre_step;
    if (abs2(test_point, start_point) > max_dis_2) return SKY_COLOR;

    float sum = 0.0;
    direction *= 0.1;
    for (int i = 0; i < 256; i++) {
        test_point += direction;
        if (cloud_min < test_point.y && test_point.y < cloud_max) {
            sum += get_cloud_noise(vec3(test_point.x, test_point.y - cloud_min, test_point.z)) * 0.01;
            // sum += get_cloud_noise(vec3(test_point.x, test_point.z, test_point.y - cloud_min)) * 0.01;
        } else if (test_point.y > cloud_max) {
            break;
        }
    }
    return SKY_COLOR + vec3(sum);
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
    return cloud(u_camera_position, getRayDirection(), 200.0 * 200.0);
}

void main() {
    float depth = texture(u_depth_texture, texcoord).r;
    vec3 color = texture(u_color_texture, texcoord).rgb;

    if (depth == 1.0f) {
        color = sky();
    }
    fragcolor = vec4(color, 1.0);
}
