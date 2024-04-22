#version 410 core

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform float u_time;

uniform sampler2D u_color_texture;

void main() {
    vec4 color = texture(u_color_texture, texcoord).rgba;

    if (texcoord.y < 1.0 / 2.0 * texcoord.x) {
        color = vec4(1.0, 0.0, 0.0, 1.0);
    }

    fragcolor = color;
}
