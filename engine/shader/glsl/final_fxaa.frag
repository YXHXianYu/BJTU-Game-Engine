#version 410 core

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform sampler2D u_color_texture;

uniform float u_time;
uniform vec2 u_resolution;
uniform int u_fxaa_mode;

// === FXAA ===

#define FXAA_CONTRAST_THRESHOLD 0.025
#define FXAA_SEARCH_TIMES 10
#define FXAA_SEARCH_LIMIT 8

// reference: https://zhuanlan.zhihu.com/p/431384101
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

    // 十字滤波器/边缘提取 
    if (u_fxaa_mode == 3) {
        return vec3(contrast * 3.0);
    } else if (u_fxaa_mode == 4) {
        if (contrast <= FXAA_CONTRAST_THRESHOLD * 2.0) {
            return vec3(0.0);
        } else {
            return color;
        }
    }

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
    float vertical = 2.0*abs(N+S-2.0*M) + abs(NE+SE-2.0*E) + abs(NW+SW-2.0*W);
    float horizontal = 2.0*abs(E+W-2.0*M) + abs(NE+NW-2.0*N) + abs(SE+SW-2.0*S);

    bool is_edge_horizontal = vertical > horizontal;

    vec2 pixel_step;
    if (is_edge_horizontal) {
        if (abs(N - M) > abs(S - M)) {
            pixel_step = vec2(0.0, delta.y);
        } else {
            pixel_step = vec2(0.0, -delta.y);
        }
    } else {
        if (abs(E - M) > abs(W - M)) {
            pixel_step = vec2(delta.x, 0.0);
        } else {
            pixel_step = vec2(-delta.x, 0.0);
        }
    }

    // 4 blend
    if (u_fxaa_mode == 1) {
        return texture(u_color_texture, texcoord + pixel_step * blend).rgb;
    } else if (u_fxaa_mode == 2) {
        float positive = abs((is_edge_horizontal ? N : E) - M);
        float negative = abs((is_edge_horizontal ? S : W) - M);
        float gradient, opposite_luminance;
        if (positive > negative) {
            gradient = positive;
            opposite_luminance = is_edge_horizontal ? N : E;
        } else {
            gradient = negative;
            opposite_luminance = is_edge_horizontal ? S : W;
        }

        // 4.1 edge blend
        vec2 texcoord_in_edge = texcoord + pixel_step * 0.5f;
        vec2 edge_step = is_edge_horizontal ? vec2(delta.x, 0.0) : vec2(0.0, delta.y);

        float edge_luminance = (M + opposite_luminance) * 0.5f;
        float gradient_threshold = edge_luminance * 0.25f;
        float p_luminance_delta, n_luminance_delta, p_distance, n_distance;
        float i;
        // positive
        for (i = 1.0; i <= FXAA_SEARCH_TIMES; i += 1.0) {
            p_luminance_delta = texture(u_color_texture, texcoord_in_edge + i * edge_step).a - edge_luminance;
            if (abs(p_luminance_delta) > gradient_threshold) {
                p_distance = i;
                break;
            }
        }
        if (i > FXAA_SEARCH_TIMES) {
            p_distance = FXAA_SEARCH_LIMIT;
        }
        // negative
        for(i = 1.0; i <= FXAA_SEARCH_TIMES; i += 1.0) {
            n_luminance_delta = texture(u_color_texture, texcoord_in_edge - i * edge_step).a - edge_luminance;
            if (abs(n_luminance_delta) > gradient_threshold) {
                n_distance = i;
                break;
            }
        }
        if (i > FXAA_SEARCH_TIMES) {
            n_distance = FXAA_SEARCH_LIMIT;
        }
        // edge blend
        float edge_blend;
        if (p_distance < n_distance) {
            if (sign(p_luminance_delta) == sign(M - edge_luminance)) {
                edge_blend = 0.0f;
            } else {
                edge_blend = 0.5f - p_distance / (p_distance + n_distance);
            }
        } else {
            if (sign(n_luminance_delta) == sign(M - edge_luminance)) {
                edge_blend = 0.0f;
            } else {
                edge_blend = 0.5f - n_distance / (p_distance + n_distance);
            }
        }

        float final_blend = max(blend, edge_blend);

        return texture(u_color_texture, texcoord + pixel_step * final_blend).rgb;
    }
}

// === Main ===

void main() {
    vec4 last_frame_rgba = texture(u_color_texture, texcoord);
    vec3 color = last_frame_rgba.rgb;
    float luminance = last_frame_rgba.a;

    if (u_fxaa_mode >= 1) {
        color = FXAA(color, luminance);
    }

    fragcolor = vec4(color, 1.0);
}
