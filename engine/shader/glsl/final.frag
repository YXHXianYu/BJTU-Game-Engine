#version 410 core

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform float u_time;

uniform sampler2D u_origin_texture;
uniform sampler2D u_blur_texture;

void main() {
    vec3 origin = texture(u_origin_texture, texcoord).rgb;
    vec3 blur = texture(u_blur_texture, texcoord).rgb;

    vec3 color = origin;
    // vec3 color = origin + blur;

    float luminance = color.r * 0.213 + color.g * 0.715 * color.b * 0.072; // luminance is needed in next pass (fxaa)

    fragcolor = vec4(color, luminance);
}
