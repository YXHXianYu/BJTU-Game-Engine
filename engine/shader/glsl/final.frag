#version 410 core

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform float u_time;

uniform sampler2D u_origin_texture;
uniform sampler2D u_blur_texture;

float A = 0.22;
float B = 0.30;
float C = 0.10;
float D = 0.20;
float E = 0.01;
float F = 0.30;
float W = 11.2;
 
vec3 uncharted2TonemapFunction(vec3 x) {
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 uncharted2Tonemap(vec3 color) {
    color = pow(color, vec3(2.1)); // contrast
    color = color * 4.0; // exposure
    return uncharted2TonemapFunction(color) / uncharted2TonemapFunction(vec3(W));
}

void main() {
    vec3 origin = texture(u_origin_texture, texcoord).rgb;
    vec3 blur = texture(u_blur_texture, texcoord).rgb;

    // vec3 color = origin;
    // vec3 color = blur;
    vec3 color = origin + blur;
    color = uncharted2Tonemap(color);

    float luminance = color.r * 0.213 + color.g * 0.715 * color.b * 0.072; // luminance is needed in next pass (fxaa)

    fragcolor = vec4(color, luminance);
}
