#version 410 core

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform sampler2D u_color_texture;

uniform float u_time;
uniform vec2 u_resolution;
uniform int u_is_enabled_fxaa;

// === FXAA ===

#define FXAA_CONTRAST_THRESHOLD 0.1

vec3 FXAA(vec3 color, float M) {
    // 1. check contrast
    vec2 delta = 1.0 / u_resolution;
    float S = texture(u_color_texture, texcoord + vec2(0.0, -delta.y)).a;
    float N = texture(u_color_texture, texcoord + vec2(0.0, delta.y)).a;
    float W = texture(u_color_texture, texcoord + vec2(-delta.x, 0.0)).a;
    float E = texture(u_color_texture, texcoord + vec2(delta.x, 0.0)).a;

    float max_luminance = max(M, max(S, max(N, max(W, E))));
    float min_luminance = min(M, min(S, min(N, min(W, E))));
    float contrast = max_luminance - min_luminance;

    // 去除下面的注释，然后你就可以看到一个十字滤波器/边缘提取的效果 
    // return vec3(contrast * 3.0);

    if (contrast <= FXAA_CONTRAST_THRESHOLD) {
        // return vec3(0.0);
        return color;
    }

    // 2. get luminance
    float NE = texture(u_color_texture, texcoord + delta).a;
    float NW = texture(u_color_texture, texcoord + vec2(-delta.x, delta.y)).a;
    float SW = texture(u_color_texture, texcoord - delta).a;
    float SE = texture(u_color_texture, texcoord + vec2(delta.x, -delta.y)).a;

    float blend = abs((2.0 * (N + E + S + W) + NE + NW + SE + SW) / 12.0 - M) / contrast;
    blend = smoothstep(0.0, 1.0, blend);
    blend = blend * blend;

    // 3. edge direction
    float vertical = 2*abs(N+S-2*M) + abs(NE+SE-2*E) + abs(NW+SW-2*W);
    float horizontal = 2*abs(E+W-2*M) + abs(NE+NW-2*N) + abs(SE+SW-2*S);

    bool is_edge_horizontal = vertical > horizontal;

    vec2 step;
    // float gradient;
    // float opposite_luminance;

    if (is_edge_horizontal) {
        if (abs(N - M) > abs(S - M)) {
            step = vec2(0.0, delta.y);
        } else {
            step = vec2(0.0, -delta.y);
        }
    } else {
        if (abs(E - M) > abs(W - M)) {
            step = vec2(delta.x, 0.0);
        } else {
            step = vec2(-delta.x, 0.0);
        }
    }

    // 4. blend
    return texture(u_color_texture, texcoord + step / 4.0).rgb;
    // TODO: https://zhuanlan.zhihu.com/p/431384101

}

// === Main ===

void main() {
    vec4 last_frame_rgba = texture(u_color_texture, texcoord);
    vec3 color = last_frame_rgba.rgb;
    float luminance = last_frame_rgba.a;

    if (u_is_enabled_fxaa == 1) {
        color = FXAA(color, luminance);
    }

    fragcolor = vec4(color, 1.0);
}
