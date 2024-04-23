
float abs2(vec2 a) {
    return a.x * a.x + a.y * a.y;
}

float abs(vec2 a) {
    return sqrt(abs2(a));
}

float abs2(vec3 a) {
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

float abs(vec3 a) {
    return sqrt(abs2(a));
}
