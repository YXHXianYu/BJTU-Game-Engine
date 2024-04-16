#ifdef BLOCK_SHADER
#include "./glsl/material_id.glsl"

layout (location = 0) in vec3 normal;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 frag_pos;
layout (location = 3) flat in int material_id;

uniform sampler3D u_block_texture;
#endif

#ifdef MODEL_SHADER
layout (location = 0) in vec3 normal;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 frag_pos;
#endif

layout(location = 0) out vec4 fragcolor;

struct SpotLight {
    vec3 pos;
    vec3 color;
};

struct DirLight {
    vec3 dir;
    vec3 color;
};

uniform int u_texture_cnt;
uniform sampler2D u_texture_diffuse;
// uniform sampler2D u_texture_specular;

uniform int u_spotlights_cnt;
uniform SpotLight u_spotlights[10];

uniform int u_dirlights_cnt;
uniform DirLight u_dirlights[10];

uniform vec3 u_cam_pos;

uniform float u_time;

uniform int u_render_by_depth;

vec3 calc_dirlight(DirLight light) {
    // ambient
    float ambient_strength = 0.1;
    vec3 ambient = ambient_strength * light.color;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(-light.dir);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light.color;

    // specular
    float specular_strength = 0.4;
    vec3 cam_dir = normalize(u_cam_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, normal);

    float specular = pow(max(dot(cam_dir, reflect_dir), 0.0), 128); // TODO: add shininess to texture

    vec3 res = ambient + diffuse + specular;

    return res;
}

vec3 calc_spotlight(SpotLight light) {
    // ambient
    float ambient_strength = 0.4;
    vec3 ambient = ambient_strength * light.color;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(light.pos - frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light.color;

    // specular
    float specular_strength = 0.5;
    vec3 cam_dir = normalize(u_cam_pos - frag_pos);
    vec3 reflect_dir = reflect(-cam_dir, norm);

    float spec = pow(max(dot(cam_dir, reflect_dir), 0.0), 32);
    vec3 specular = specular_strength * spec * light.color;

    vec3 res = ambient + diffuse + specular;

    // attenuation
    float dis = length(light.pos - frag_pos);
    float attenuation = 1.0 / (1 + 0.09 * dis + 0.032 * (dis * dis)); 

    return res * attenuation;
}

void main() {

    if (u_render_by_depth == 1) {

        float v = pow(1.0 - gl_FragCoord.z, 1.0 / 2.0);
        fragcolor = vec4(vec3(v), 1.0);

        return;
    }

    vec3 light = vec3(0.0);
    for (int i = 0; i < u_spotlights_cnt; i++) {
        light += calc_spotlight(u_spotlights[i]);
    }
    for (int i = 0; i < u_dirlights_cnt; i++) {
        light += calc_dirlight(u_dirlights[i]);
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
