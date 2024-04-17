#version 410 core

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform float u_time;

uniform sampler2D u_color_texture;
uniform sampler2D u_depth_texture;


vec4 sky() {
    return vec4(0.53, 0.81, 0.92, 1.0);
}

void main() {
    float depth = texture(u_depth_texture, texcoord).r;
    vec4 color = texture(u_color_texture, texcoord).rgba;

    if (depth == 1.0f) {
        color = sky();
    }
    fragcolor = color;
}
