#version 410 core

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform float u_time;

uniform sampler2D u_color_texture;
uniform sampler2D u_depth_texture;

// === Cloud ===

#define CLOUD_MIN (30.0)
#define CLOUD_MAX (40.0)
#define SKY_COLOR (vec3(0.53, 0.81, 0.92))

bool is_cloud(vec3 p) {
    return -10.0 < p.x && p.x < 10.0
        && -10.0 < p.z && p.z < 10.0
        && CLOUD_MIN < p.y && p.y < CLOUD_MAX;
}

vec3 cloud(vec3 start_point, vec3 direction, float max_dis) {
    vec3 cur_point = start_point;
    float sum = 0.0;
    direction = normalize(direction) * 0.1;
    for (int i = 0; i < 256; i++) {
        cur_point += direction;
        if (is_cloud(cur_point)) {
            sum += 0.01;
        }
    }
    return SKY_COLOR + vec3(sum);
}

vec3 sky() {
    // return cloud(..);
    return SKY_COLOR;
}

void main() {
    float depth = texture(u_depth_texture, texcoord).r;
    vec3 color = texture(u_color_texture, texcoord).rgb;

    if (depth == 1.0f) {
        color = sky();
    }
    fragcolor = vec4(color, 1.0);
}
