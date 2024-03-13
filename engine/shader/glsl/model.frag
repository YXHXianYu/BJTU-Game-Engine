#version 410 core

layout (location = 0) in vec3 normal;
layout (location = 1) in vec2 texcoord;

layout(location = 0) out vec4 fragcolor;

uniform sampler2D u_texture_diffuse;
// uniform sampler2D u_texture_specular;

uniform float u_time;

void main() {
    vec4 tex_color = texture(u_texture_diffuse, texcoord);

    fragcolor = vec4(tex_color.rgb, 1.0f);
}
