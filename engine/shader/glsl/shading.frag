#version 410 core

#include "./include/config.glsl"
#include "./include/utils.glsl"

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform sampler2D u_gbuffer_position;
uniform sampler2D u_gbuffer_normal;
uniform sampler2D u_gbuffer_color;
uniform sampler2D u_gbuffer_transparent;
uniform sampler2D u_depth_texture;

uniform float u_time;
uniform vec3 u_camera_position;

uniform int u_is_enable_depth_rendering;

// === Lights ===

struct SpotLight {
    vec3 pos;
    vec3 color;
};

struct DirLight {
    vec3 dir; // we have normalized this vector in render_directed_light.h
    vec3 color;
};

uniform int u_spotlights_cnt;
uniform SpotLight u_spotlights[10];

uniform int u_dirlights_cnt;
uniform DirLight u_dirlights[10];

void calc_light(
    vec3 frag_pos, vec3 normal, vec3 light_dir, vec3 light_color,
    out vec3 diffuse, out vec3 specular
) {
    diffuse = max(dot(normal, light_dir), 0.0) * light_color;

    // light_dir = normalize(light_dir);
    if (dot(light_dir, normal) > 0.0) {
        vec3 cam_dir = normalize(u_camera_position - frag_pos);
        vec3 reflect_dir = reflect(-light_dir, normal);
        specular = pow(max(dot(cam_dir, reflect_dir), 0.0), 64) * light_color;
    } else {
        specular = vec3(0.0);
    }
}

// === Shadow Map ===

uniform int u_shadow_mode;
uniform sampler2D u_shadow_texture;
uniform float u_shadow_map_width;
uniform float u_shadow_map_height;

uniform mat4 u_light_space_matrix;
uniform float u_light_space_near;
uniform float u_light_space_far;

#define PCF_FILTER_SIZE 13
#define PCSS_BLOCKER_SEARCH_SIZE 41
#define PCSS_W_LIGHT_SIZE 50.0
#define PCSS_SAMPLE_STEP_MULTIPLIER 0.25
#define PCSS_PENUMBRA_LIMIT 21.0
#define VSM_BLOCKER_SEARCH_SIZE_LOG2 4.0 // (2^2)^2 = 4 * 4
#define VSM_W_LIGHT_SIZE 10.0

float rand(float x, float y) {
    return fract(sin(12.9898 * x + 78.233 * y) * 43758.5453);
}

float calc_shadow(vec4 frag_pos_light_space) {
    vec3 projection_pos = frag_pos_light_space.xyz / frag_pos_light_space.w;
    projection_pos = projection_pos * 0.5 + 0.5;
    if (projection_pos.z > 1.0) { return 0.0; }
    projection_pos.z = depth_linearize(projection_pos.z, u_light_space_near, u_light_space_far);

    float depth_in_frag = projection_pos.z;
    vec2 shadow_map_size = vec2(u_shadow_map_width, u_shadow_map_height);

    if (u_shadow_mode == 1 || u_shadow_mode == 2) {
        int filter_size = u_shadow_mode == 2 ? PCF_FILTER_SIZE : 1;
        int filter_min = -filter_size / 2;
        int filter_max = filter_size / 2;

        float shadow = 0.0;
        for (int i = filter_min; i <= filter_max; i++) {
            for (int j = filter_min; j <= filter_max; j++) {
                float depth_in_shadow_map = texture(u_shadow_texture, (projection_pos.xy) + vec2(i, j) / shadow_map_size).r;
                shadow += ((depth_in_frag > depth_in_shadow_map + SHADOW_MAP_EPS) ? 1.0 : 0.0);
            }
        }

        return shadow / float(filter_size * filter_size);
    } else if (u_shadow_mode == 3) { // PCSS, Percentage Closer Soft Shadows
        // Step 1 - Blocker Search
        int filter_size = PCSS_BLOCKER_SEARCH_SIZE;
        int filter_min = -filter_size / 2;
        int filter_max = filter_size / 2;
        float z_occ = 0.0;
        float z_occ_cnt = 0.0;
        float z_unocc = 0.0;
        float z_unocc_cnt = 0.0;
        for (int i = filter_min; i <= filter_max; i++) {
            for (int j = filter_min; j <= filter_max; j++) {
                float depth_in_shadow_map
                    = texture(u_shadow_texture, (projection_pos.xy) + vec2(i, j) / shadow_map_size).r;
                if (depth_in_frag > depth_in_shadow_map + SHADOW_MAP_EPS) {
                    // occluded
                    z_occ += depth_in_shadow_map;
                    z_occ_cnt += 1.0;
                } else {
                    // unoccluded
                    z_unocc += depth_in_shadow_map;
                    z_unocc_cnt += 1.0;
                }
            }
        }
        if (z_occ_cnt < SHADOW_MAP_EPS) {   // all unoccluded
            return 0.0;
        }
        if (z_unocc_cnt < SHADOW_MAP_EPS) { // all occluded
            return 1.0;
        }
        z_occ /= z_occ_cnt;
        z_unocc /= z_unocc_cnt;
        if (z_unocc < z_occ) {
            return 0.0;
        }

        // Step 2 - Penumbra Estimation
        float w_penumbra = max(min((z_unocc - z_occ) * PCSS_W_LIGHT_SIZE / z_occ, PCSS_PENUMBRA_LIMIT) / PCSS_SAMPLE_STEP_MULTIPLIER, 1.0);

        // return w_penumbra / 25.0;

        // Step 3 - PCF
        filter_size = int(w_penumbra - 1.0) / 2 * 2 + 1;
        filter_max = filter_size / 2 + 1;
        filter_min = -filter_max;

        // used to normalize the shadow value
        float penumbra_coef_numer = w_penumbra * w_penumbra - float(filter_size * filter_size);
        float penumbra_coef_denom = float((filter_size + 2) * (filter_size + 2) - filter_size * filter_size);
        float penumbra_coef = penumbra_coef_numer / penumbra_coef_denom;
        // float penumbra_coef = w_penumbra * w_penumbra / float((filter_size + 2) * (filter_size + 2));

        float shadow = 0.0;
        for (int i = filter_min; i <= filter_max; i++) {
            for (int j = filter_min; j <= filter_max; j++) {
                float depth_in_shadow_map
                    = texture(u_shadow_texture, (projection_pos.xy) + vec2(i, j) / shadow_map_size * PCSS_SAMPLE_STEP_MULTIPLIER).r;
                shadow += ( (depth_in_frag > depth_in_shadow_map + SHADOW_MAP_EPS) ? (
                        (i == filter_min || i == filter_max || j == filter_min || j == filter_max)
                        ? penumbra_coef
                        : 1.0
                        // penumbra_coef
                    ) : 0.0
                );
            }
        }
        shadow = shadow / (w_penumbra * w_penumbra);
        return shadow;

    } else if (u_shadow_mode == 4) { // VSM, Variance Shadow Mapping
        // Core: N1/N * z_unocc + N2/N * z_occ = z_avg
        // Chebychev's inequality: (Let t > mu)
        //     P(x > t) <= sigma^2 / (sigma^2 + (t - mu)^2)

        // Step 1 - Blocker Search (VSM)
        vec2 depInfo = textureLod(u_shadow_texture, projection_pos.xy, VSM_BLOCKER_SEARCH_SIZE_LOG2).rg;

        float z_unocc = projection_pos.z;
        float z_avg = depInfo.x;
        if (!(z_unocc > z_avg + SHADOW_MAP_EPS)) {
            return 0.0;
        }
        // z_unocc > z_avg
        float var = depInfo.y - depInfo.x * depInfo.x;
        if (var <= 0) {
            return 1.0;
        }

        float N1N = var / (var + (projection_pos.z - depInfo.x) * (projection_pos.z - depInfo.x));
        float N2N = 1.0 - N1N;
        float z_occ = (z_avg - N1N * z_unocc) / N2N;

        if (z_occ + SHADOW_MAP_EPS >= z_avg) {
            return 1.0;
        }

        // Step 2 - Penumbra Estimation
        float w_penumbra = max((z_unocc - z_occ) * VSM_W_LIGHT_SIZE / z_occ, 1.0);

        // Step 3 - PCF (VSM)
        vec2 depInfo2 = textureLod(u_shadow_texture, projection_pos.xy, log2(w_penumbra)).rg;
        float var2 = depInfo2.y - depInfo2.x * depInfo2.x;
        float shadow = 1.0 - var2 / (var2 + (projection_pos.z - depInfo2.x) * (projection_pos.z - depInfo2.x));
        
        return shadow;
    } else {
        return 0.0;
    }
}

// === Main ===

vec3 shading() {
    vec3  frag_pos = texture(u_gbuffer_position, texcoord).rgb;
    vec3  normal   = texture(u_gbuffer_normal, texcoord).rgb;
    vec3  kd       = texture(u_gbuffer_color, texcoord).rgb;
    float ks       = texture(u_gbuffer_color, texcoord).a;
    float depth    = texture(u_depth_texture, texcoord).r;
    if (depth == 1.0) {
        return vec3(0.0);
    }

    vec3  transparent_color = texture(u_gbuffer_transparent, texcoord).rgb;
    float transparent_alpha = texture(u_gbuffer_transparent, texcoord).a;
    if (transparent_alpha > 1.0 + EPS) {
        transparent_alpha -= 1.0;
        kd = mix(kd, transparent_color, transparent_alpha);
    } else if (transparent_alpha > EPS) {
        kd = mix(kd, transparent_color, transparent_alpha);
        ks = min(1.0, ks * 1.75); // magic formula, make sun light reflected by water stronger
    }

    vec3 ambient  = AMBIENT_STRENGTH * kd * u_dirlights[0].color; // u_dirlights[0] is the sun
    vec3 diffuse  = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (int i = 0; i < u_dirlights_cnt; i++) {
        vec3 diffuse_light, specular_light;

        calc_light(frag_pos, normal, -u_dirlights[i].dir, u_dirlights[i].color, diffuse_light, specular_light);

        diffuse += diffuse_light;
        specular += specular_light;
    }
    for (int i = 0; i < u_spotlights_cnt; i++) {
        vec3 diffuse_light, specular_light;

        vec3 light_dir = normalize(u_spotlights[i].pos - frag_pos);
        float dis = length(u_spotlights[i].pos - frag_pos);
        float attenuation = 1.0 / (1 + 0.09 * dis + 0.032 * (dis * dis)); 
        calc_light(frag_pos, normal, light_dir, u_spotlights[i].color * attenuation, diffuse_light, specular_light);

        diffuse += diffuse_light;
        specular += specular_light;
    }

    float shadow;
    if (u_shadow_mode == 0) {
        shadow = 0.0;
    } else {
        shadow = calc_shadow(u_light_space_matrix * vec4(frag_pos, 1.0));
    }

    return ambient + (1.0 - shadow) * (kd * diffuse * DIFFUSE_STRENGTH + ks * specular);
}

void main() {
    fragcolor = vec4(shading(), 1.0);
    // float depth = textureLod(u_shadow_texture, texcoord, 0.0).r;
    // fragcolor = vec4(vec3(depth), 1.0);
}
