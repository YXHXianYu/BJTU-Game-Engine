#version 410 core

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform float u_time;

uniform sampler2D u_color_texture;
uniform sampler2D u_depth_texture;


vec4 sky() {
    return vec4(1.0, 0.0, 0.0, 1.0);
    return vec4( sin(u_time)*0.5+0.5, cos(u_time)*0.5+0.5, 0.0, 1.0);
}

void main() {
    float depth = texture(u_depth_texture, texcoord).r;
    vec4 color = texture(u_color_texture, texcoord).rgba;

    if (depth > 0.8f) {
        color = sky();
    }
    fragcolor = color;

    fragcolor = vec4(vec3(depth), 1.0f);
}
