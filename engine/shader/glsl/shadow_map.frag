#version 410 core

#include "./include/utils.glsl"

layout (location = 0) out vec2 fragcolor;

uniform float u_far;
uniform float u_near;

void main() {
    // gl_FragDepth = gl_FragCoord.z;
    // float depth = (2.0 * u_near) / (u_far + u_near - gl_FragCoord.z * (u_far - u_near));
    float depth = depth_linearize(gl_FragCoord.z, u_near, u_far);
    fragcolor = vec2(depth, depth * depth);
}