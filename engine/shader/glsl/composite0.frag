#version 410 core

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform sampler2D u_color_texture;
uniform sampler2D u_depth_texture;
uniform sampler2D u_gbuffer_position;

uniform vec3 u_camera_position;
uniform vec2 u_resolution;
uniform mat4 u_inverse_view;
uniform mat4 u_inverse_projection;

// === Cloud ===

#define CLOUD_MIN (20.0)
#define CLOUD_MAX (30.0)
#define SKY_COLOR (vec3(0.47, 0.66, 1.00))

bool is_cloud(vec3 p) {
    return -10.0 < p.x && p.x < 10.0
        && -10.0 < p.z && p.z < 10.0
        && CLOUD_MIN < p.y && p.y < CLOUD_MAX;
}

vec3 cloud(vec3 start_point, vec3 direction, float max_dis) {
    vec3 cur_point = start_point;
    float sum = 0.0;
    direction = normalize(direction) * 0.2;
    for (int i = 0; i < 256; i++) {
        cur_point += direction;
        if (is_cloud(cur_point)) {
            sum += 0.01;
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
    return cloud(u_camera_position, getRayDirection(), 200.0);
}

void main() {
    float depth = texture(u_depth_texture, texcoord).r;
    vec3 color = texture(u_color_texture, texcoord).rgb;

    if (depth == 1.0f) {
        color = sky();
    }
    fragcolor = vec4(color, 1.0);
}
