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

// === Cloud & Fog ===

#define CLOUD_MIN   (40.0)
#define SKY_COLOR (vec3(0.47, 0.66, 1.00))
// #define FOG_COLOR (vec3(0.38, 0.47, 0.59))
#define FOG_COLOR (SKY_COLOR)


#define CLOUD_NOISE_SPEED (vec2(5.0, 0.0))
#define CLOUD_NOISE_SCALE (0.02) // 值越小，单朵云越大，但视野内云越少
#define CLOUD_NOISE_THRESHOLD (0.5)
#define CLOUD_STEP_LENGTH (0.5)
#define CLOUD_SAMPLE_TIMES (128)

float get_cloud_noise(vec3 p) {
    vec3 q = p - vec3(0.0, 0.0, 1.0) * u_time;
    q *= CLOUD_NOISE_SCALE;
    float f;
    f  = 0.50000 * noise_hash(q); q = q*2.02;    
    f += 0.25000 * noise_hash(q); q = q*2.03;    
    f += 0.12500 * noise_hash(q); q = q*2.01;    
    f += 0.06250 * noise_hash(q); q = q*2.02;    
    f += 0.03125 * noise_hash(q);
    f = clamp(1.0 * f + min(p.y / 40.0, 0.5) - 0.5, 0.0, 1.0);
    f = max(f - CLOUD_NOISE_THRESHOLD, 0.0) * (1.0 / (1.0 - CLOUD_NOISE_THRESHOLD));
    return f;
}

// refer to iq's Clouds: https://www.shadertoy.com/view/XslGRr
vec3 cloud(vec3 start_point, vec3 direction) {
    if (direction.y <= 0.1) return SKY_COLOR;

    vec4 sum = vec4(0.0);
    float cloud_min = start_point.y + CLOUD_MIN * (exp(-start_point.y / CLOUD_MIN) + 0.001);
    float t = 0.05 + ((cloud_min - start_point.y) / direction.y);

    for (int i = 0; i < 64; i++) {
        vec3 pos = start_point + direction * t;

        if (sum.a > 0.99) break;

        float density = get_cloud_noise(vec3(pos.x, pos.y - cloud_min + CLOUD_MIN, pos.z));

        if (density > 0.01) {
            pos = pos + u_sunlight_direction * 0.3;
            float density2 = get_cloud_noise(vec3(pos.x, pos.y - cloud_min + CLOUD_MIN, pos.z));

            float diff = clamp((density - density2) / 0.6, 0.0, 1.0);
            vec3 lighting = vec3(1.0, 0.6, 0.3) * diff + vec3(0.91, 0.98, 1.05);
            vec4 color = vec4(mix(vec3(1.0, 0.95, 0.8), vec3(0.25, 0.3, 0.35), density), density);
            color.rgb *= lighting;
            color.w *= 0.4;
            color.rgb *= color.w;
            sum += color * (1.0 - sum.w);
        }

        t += 0.5;
    }

    sum = clamp(sum, 0.0, 1.0);
    return SKY_COLOR * (1.0 - sum.a) + sum.rgb;
}

// designed & created by me, YXHXianYu
vec4 fog_sky(vec3 start_point, vec3 direction) {
    if (direction.y <= -0.2 || direction.y >= 0.4) return vec4(0.0);

    if (direction.y >= 0.1) {
        return vec4(FOG_COLOR, smoothstep(0.0, 1.0, (0.4 - direction.y) / 0.3));
    } else {
        return vec4(FOG_COLOR, 1.0);
    }
}

vec4 fog(vec3 start_point, vec3 direction, float dis) {
    if (direction.y <= -0.2 || direction.y >= 0.4) return vec4(0.0);

    return vec4(FOG_COLOR, smoothstep(0.0, 1.0, (dis - 30.0) / 10.0));
}

void main() {
    float depth   = texture(u_depth_texture, texcoord).r;
    vec3 color    = texture(u_color_texture, texcoord).rgb;
    vec3 frag_pos = texture(u_gbuffer_position, texcoord).xyz;
    vec3 ray_direction = getRayDirection();

    if (depth == 1.0f) {
        color = cloud(u_camera_position, ray_direction);
        vec4 f = fog_sky(u_camera_position, ray_direction);
        color = mix(color, f.rgb, f.a);
    }

    float dis = abs(frag_pos - u_camera_position);
    if (dis >= 30.0) {
        vec4 f = fog(u_camera_position, ray_direction, dis);
        color = mix(color, f.rgb, f.a);
    }
    fragcolor = vec4(color, 1.0);
}
