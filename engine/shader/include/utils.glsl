
float abs2(vec3 a) {
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

float abs2_vec3(vec3 a) {
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

float abs2_vec2(vec2 a) {
    return a.x * a.x + a.y * a.y;
}

float sqrt1(float x) {
    return x * (2.0 - x);
}

float depth_linearize(float z, float near, float far) {
    return (2.0 * near) / (far + near - z * (far - near));
}