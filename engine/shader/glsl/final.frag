#version 410 core

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform float u_time;

uniform sampler2D u_origin_texture;
uniform sampler2D u_blur_texture;

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 13.134;
 
vec3 uncharted2Tonemap(vec3 x) {
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main() {
    vec3 origin = texture(u_origin_texture, texcoord).rgb;
    vec3 blur = texture(u_blur_texture, texcoord).rgb;

    // vec3 color = origin;
    // vec3 color = blur;
    vec3 color = origin + blur;
    color = pow(color, vec3(1.4));
    color *= 6.0;
    vec3 curr = uncharted2Tonemap(color);
    vec3 whiteScale = 1.0f / uncharted2Tonemap(vec3(W));
    color = curr * whiteScale;

    float luminance = color.r * 0.213 + color.g * 0.715 * color.b * 0.072; // luminance is needed in next pass (fxaa)

    fragcolor = vec4(color, luminance);
}
