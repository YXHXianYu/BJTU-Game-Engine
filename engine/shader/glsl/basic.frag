#version 410 core

layout (location = 0) in vec3 normal;
layout (location = 1) in vec2 texcoord;

layout(location = 0) out vec4 fragcolor;

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;

uniform float u_time;
uniform vec2 u_resolution;

float scale_texcoord_x_to_square(in float x) {
    return (x - (u_resolution.x - u_resolution.y) / (2.0 * u_resolution.x)) * (u_resolution.x / u_resolution.y);
}

void main() {
    vec4 tex_color0 = texture(u_texture0, texcoord);

    float square_texcoord_x = scale_texcoord_x_to_square(texcoord.x);

    vec4 tex_color1;
    if (square_texcoord_x >= 0.0 && square_texcoord_x <= 1.0)
        tex_color1 = texture(u_texture1, vec2(square_texcoord_x, texcoord.y));
    else
        tex_color1 = vec4(0.0, 0.0, 0.0, 0.0);
    
    fragcolor = tex_color1.a == 0.0 ? tex_color0 : tex_color1;
}