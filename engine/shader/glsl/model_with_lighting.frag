#version 410 core

layout (location = 0) in vec3 normal;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 FragPos;

layout(location = 0) out vec4 fragcolor;

uniform sampler2D u_texture_diffuse;
// uniform sampler2D u_texture_specular;
uniform vec3 u_light_color;
uniform vec3 u_light_pos;
uniform vec3 u_cam_pos;

uniform float u_time;

void main() {
    // ambient
    float ambient_strength = 0.2;
    vec3 ambient = ambient_strength * u_light_color;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(u_light_pos - FragPos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * u_light_color;

    // specular
    float specular_strength = 0.5;
    vec3 cam_dir = normalize(u_cam_pos - FragPos);
    vec3 reflect_dir = reflect(-cam_dir, norm);

    float spec = pow(max(dot(cam_dir, reflect_dir), 0.0), 32);
    vec3 specular = specular_strength * spec * u_light_color;

    vec3 light = ambient + diffuse + specular;

    vec4 tex_color = texture(u_texture_diffuse, texcoord);
    fragcolor = vec4(light * tex_color.rgb, 1.0f);
}