#version 410 core

layout (location = 0) in vec2 texcoord;

layout (location = 0) out vec4 fragcolor;

uniform float u_time;

uniform sampler2D u_origin_texture;
uniform sampler2D u_blur_texture;

float A = 0.15;
float B = 0.45;
float C = 0.10;
float D = 0.20;
float E = 0.01;
float F = 0.30;
float W = 13.134;
 
vec3 uncharted2TonemapFunction(vec3 x) {
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 uncharted2Tonemap(vec3 color) {
    color = pow(color, vec3(2.53)); // contrast
    color = color * 15.0; // exposure
    color = pow(color, vec3(1.0 / 1.3));

    color = uncharted2TonemapFunction(color) / uncharted2TonemapFunction(vec3(W));
    return color;
}

float GetLuminance(vec3 color) {
	return dot(color, vec3(0.299, 0.587, 0.114));
}

float CG_RR = 255;
float CG_RG = 0;
float CG_RB = 0;
float CG_RI = 1.00;
float CG_RM = 0;
float CG_RC = 1.00;

float CG_GR = 0;
float CG_GG = 255;
float CG_GB = 0;
float CG_GI = 1.00;
float CG_GM = 0;
float CG_GC = 1.00;

float CG_BR = 0;
float CG_BG = 0;
float CG_BB = 255;
float CG_BI = 1.00;
float CG_BM = 0;
float CG_BC = 1.00;

float CG_TR = 255;
float CG_TG = 255;
float CG_TB = 255;
float CG_TI = 1.00;
float CG_TM = 0.0;

void ColorGrading(inout vec3 color) {
	vec3 cgColor = pow(color.r, CG_RC) * pow(vec3(CG_RR, CG_RG, CG_RB) / 255.0, vec3(2.2)) +
				   pow(color.g, CG_GC) * pow(vec3(CG_GR, CG_GG, CG_GB) / 255.0, vec3(2.2)) +
				   pow(color.b, CG_BC) * pow(vec3(CG_BR, CG_BG, CG_BB) / 255.0, vec3(2.2));
	vec3 cgMin = pow(vec3(CG_RM, CG_GM, CG_BM) / 255.0, vec3(2.2));
	color = (cgColor * (1.0 - cgMin) + cgMin) * vec3(CG_RI, CG_GI, CG_BI);
	
	vec3 cgTint = pow(vec3(CG_TR, CG_TG, CG_TB) / 255.0, vec3(2.2)) * GetLuminance(color) * CG_TI;
	color = mix(color, cgTint, CG_TM);
}

float SATURATION = 1.00;
float VIBRANCE = 1.00;

void ColorSaturation(inout vec3 color) {
	float grayVibrance = (color.r + color.g + color.b) / 3.0;
	float graySaturation = grayVibrance;
	if (SATURATION < 1.00) graySaturation = dot(color, vec3(0.299, 0.587, 0.114));

	float mn = min(color.r, min(color.g, color.b));
	float mx = max(color.r, max(color.g, color.b));
	float sat = (1.0 - (mx - mn)) * (1.0 - mx) * grayVibrance * 5.0;
	vec3 lightness = vec3((mn + mx) * 0.5);

	color = mix(color, mix(color, lightness, 1.0 - VIBRANCE), sat);
	color = mix(color, lightness, (1.0 - lightness) * (2.0 - VIBRANCE) / 2.0 * abs(VIBRANCE - 1.0));
	color = color * SATURATION - graySaturation * (SATURATION - 1.0);
}

float EXPOSURE = -0.2;
float TONEMAP_LOWER_CURVE = 2.3;
float TONEMAP_UPPER_CURVE = 2.3;
float TONEMAP_WHITE_CURVE = 2.3;

void BSLTonemap(inout vec3 color) {
	color = color * exp2(2.0 + EXPOSURE);
	color = color / pow(pow(color, vec3(TONEMAP_WHITE_CURVE)) + 1.0, vec3(1.0 / TONEMAP_WHITE_CURVE));
	color = pow(color, mix(vec3(TONEMAP_LOWER_CURVE), vec3(TONEMAP_UPPER_CURVE), sqrt(color)));
}

void main() {
    vec3 origin = texture(u_origin_texture, texcoord).rgb;
    vec3 blur = texture(u_blur_texture, texcoord).rgb;

    // vec3 color = origin;
    // vec3 color = blur;
    vec3 color = origin + blur;
    ColorGrading(color);
    BSLTonemap(color);
    ColorSaturation(color);

    // color = uncharted2Tonemap(color);

    float luminance = color.r * 0.213 + color.g * 0.715 * color.b * 0.072; // luminance is needed in next pass (fxaa)

    fragcolor = vec4(color, luminance);
}
