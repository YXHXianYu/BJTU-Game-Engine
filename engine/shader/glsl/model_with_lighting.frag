#version 410 core

layout (location = 0) in vec3 normal;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 FragPos;

layout(location = 0) out vec4 fragcolor;

struct SpotLight {
    vec3 color;
    vec3 pos;
};

uniform sampler2D u_texture_diffuse;
// uniform sampler2D u_texture_specular;
uniform int u_spotlights_cnt;
uniform SpotLight u_spotlights[10];
// uniform vec3 u_light_color;
// uniform vec3 u_light_pos;
uniform vec3 u_cam_pos;

uniform float u_time;

vec3 calc_spotlight(SpotLight spotlight) {
    // ambient
    float ambient_strength = 0.1;
    vec3 ambient = ambient_strength * spotlight.color;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(spotlight.pos - FragPos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * spotlight.color;

    // specular
    float specular_strength = 0.5;
    vec3 cam_dir = normalize(u_cam_pos - FragPos);
    vec3 reflect_dir = reflect(-cam_dir, norm);

    float spec = pow(max(dot(cam_dir, reflect_dir), 0.0), 32);
    vec3 specular = specular_strength * spec * spotlight.color;

    vec3 light = ambient + diffuse + specular;

    // attenuation
    float dis = length(spotlight.pos - FragPos);
    float attenuation = 1.0 / (1 + 0.09 * dis + 0.032 * (dis * dis)); 

    return light * attenuation;
}

void main() {
    vec3 light = vec3(0.0);
    for (int i = 0; i < u_spotlights_cnt; i++) {
        light += calc_spotlight(u_spotlights[i]);
    }
    // maybe needs to do a clamp on the light to 0.0~1.0?

    vec4 tex_color = texture(u_texture_diffuse, texcoord);
    fragcolor = vec4(light * tex_color.rgb, 1.0);
}