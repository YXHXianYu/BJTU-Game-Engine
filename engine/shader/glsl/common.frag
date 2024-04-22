
#include "./include/config.glsl"

#ifdef BLOCK_SHADER
layout (location = 4) flat in int material_id;

uniform sampler3D u_block_texture;
#endif

#ifdef MODEL_SHADER
#endif

layout (location = 0) in vec3 normal;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 frag_pos;

layout (location = 0) out vec3 gbuffer_position;
layout (location = 1) out vec3 gbuffer_normal;
layout (location = 2) out vec4 gbuffer_color;

uniform float u_time;

uniform int u_texture_cnt;
uniform sampler2D u_texture_diffuse;
// uniform sampler2D u_texture_specular;

void main() {
    if (u_render_by_depth == 1) {
        float v = pow(1.0 - gl_FragCoord.z, 1.0 / 3.0);
        fragcolor = vec4(vec3(v), 1.0);
        return;
    }


    float shadow = (u_is_enable_shadow_map > EPS ? shadow_calculate(frag_pos_light_space) : 0.0);

    vec3 light = vec3(0.0);
    for (int i = 0; i < u_spotlights_cnt; i++) {
        light += calc_spotlight(u_spotlights[i], shadow);
    }
    for (int i = 0; i < u_dirlights_cnt; i++) {
        light += calc_dirlight(u_dirlights[i], shadow);
    }
    // maybe needs to do a clamp on the light to 0.0~1.0?

#ifdef BLOCK_SHADER
    vec4 tex_color = texture(u_block_texture, vec3(texcoord, float(material_id) / float(MATERIAL_ID_SUM - 1)));
#endif

#ifdef MODEL_SHADER
    vec4 tex_color;
    if (u_texture_cnt == 0) {
        tex_color = vec4(1.0);
    } else {
        tex_color = texture(u_texture_diffuse, texcoord);
    }
#endif

    fragcolor = vec4(light * tex_color.rgb, tex_color.a);
}
