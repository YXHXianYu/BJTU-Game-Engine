
#include "./include/config.glsl"

layout (location = 0) in vec3 normal;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 frag_pos;

layout (location = 0) out vec3 gbuffer_position;
layout (location = 1) out vec3 gbuffer_normal;
layout (location = 2) out vec4 gbuffer_color;
layout (location = 3) out vec4 gbuffer_transparent;

#ifdef BLOCK_SHADER
layout (location = 3) flat in int material_id;

uniform sampler3D u_block_texture;
#endif

#ifdef MODEL_SHADER
uniform int u_texture_cnt;
uniform sampler2D u_texture_diffuse;
// uniform sampler2D u_texture_specular;
uniform vec3 u_diffuse_color;
#endif

#ifdef TRANSPARENT_SHADER
uniform vec3 u_camera_position;
uniform vec4 u_transparent_info; // rgba: color + alpha
#endif

void main() {

    gbuffer_position = frag_pos;
    gbuffer_normal = normalize(normal); // TODO: check if it's needed

#ifdef BLOCK_SHADER
    gbuffer_color.rgb = texture(u_block_texture, vec3(texcoord, float(material_id) / float(MATERIAL_ID_SUM - 1))).rgb;
    gbuffer_color.a = 0.4; // TODO: customized specular
#endif

#ifdef MODEL_SHADER
    if (u_texture_cnt == 0) {
        gbuffer_color.rgb = u_diffuse_color;
        gbuffer_color.rgb = clamp(gbuffer_color.rgb, 0.0, 1.0);
    } else {
        gbuffer_color.rgb = texture(u_texture_diffuse, texcoord).rgb;
    }
    gbuffer_color.a = 0.4; // TODO: customized specular
#endif

#ifdef TRANSPARENT_SHADER
    gbuffer_transparent = u_transparent_info;
    if (dot(normal, frag_pos - u_camera_position) >= 0) {
        gbuffer_transparent.a = 1.0 + u_transparent_info.a; // transparent but no reflection
    }
#endif
}
