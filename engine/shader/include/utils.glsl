
float abs2(vec2 a, vec2 b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

float abs(vec2 a, vec2 b) {
    return sqrt(abs2(a, b));
}

float abs2(vec3 a, vec3 b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z);
}

float abs(vec3 a, vec3 b) {
    return sqrt(abs2(a, b));
}
