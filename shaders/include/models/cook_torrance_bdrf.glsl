// GGX Microfacet sampling
vec3 sampleGGXWorld(vec3 normal, float roughness, float u1, float u2) {
    float a = roughness * roughness;
    float phi = TWO_PI * u1;

    float cosTheta = sqrt(max((1.0 - u2) / max(1.0 + (a * a - 1.0) * u2, EPSILON), 0.0));
    float sinTheta = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));

    vec3 localH = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
    return getBasis(normal) * localH;
}

// Microfacet Masking-Shadowing Function (Smith-GGX)
float smithGeometry(float NdotV, float NdotL, float roughness) {
    float a = max(roughness * roughness, EPSILON);
    float k = a * 0.5;
    float g1v = NdotV / max(NdotV * (1.0 - k) + k, EPSILON);
    float g1l = NdotL / max(NdotL * (1.0 - k) + k, EPSILON);
    return g1v * g1l;
}
