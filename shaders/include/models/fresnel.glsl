vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    float t = clamp(1.0 - cosTheta, 0.0, 1.0);
    float t2 = t * t;
    return F0 + (vec3(1.0) - F0) * (t2 * t2 * t);
}
