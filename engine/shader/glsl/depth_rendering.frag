#version 410 core

layout(location = 0) out vec4 fragcolor;

void main() {
    float z = pow(1.0 - gl_FragCoord.z, 0.5);
    fragcolor = vec4(vec3(z), 1.0f);
}
