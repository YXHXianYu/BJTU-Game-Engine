#version 410 core

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform float u_time;

uniform vec2 u_resolution;
uniform sampler2D u_color_texture;

const float offset[9] = float[] (0.0, 1.4896, 3.4757, 5.4619, 7.4482, 9.4345, 11.421, 13.4075, 15.3941);
const float weight[9] = float[] (0.066812, 0.129101, 0.112504, 0.08782, 0.061406, 0.03846, 0.021577, 0.010843, 0.004881);

vec3 highlight(vec3 color) {
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    return color.rgb * max(brightness - 0.25, 0.0);
}
 
void main() {
    vec2 uv = texcoord;
    vec3 color = highlight(texture(u_color_texture, uv).rgb) * weight[0];

    for(int i = 1; i < 9; i++)
    {
        color += highlight(texture(u_color_texture, uv + vec2(1.0, 0.0) / u_resolution * offset[i]).rgb) * weight[i];
        color += highlight(texture(u_color_texture, uv - vec2(1.0, 0.0) / u_resolution * offset[i]).rgb) * weight[i];
    }

    fragcolor = vec4(color, 1.0);
}
