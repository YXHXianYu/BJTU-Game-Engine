#version 410 core

layout (location = 0) out vec2 fragcolor;

void main() {
    // gl_FragDepth = gl_FragCoord.z;
    fragcolor = vec2(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z);
}