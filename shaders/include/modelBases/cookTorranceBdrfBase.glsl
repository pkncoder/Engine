// GGX Microfacet sampling (kinda like D)
vec3 sampleGGXWorld(vec3 normal, float roughness, float u1, float u2) {
    float a = roughness * roughness;
    float phi = TWO_PI * u1;

    float cosTheta = sqrt(max((1.0 - u2) / max(1.0 + (a * a - 1.0) * u2, EPSILON), 0.0));
    float sinTheta = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));

    vec3 localH = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
    return getBasis(normal) * localH;
}

// Microfacet Masking-Shadowing Function (Smith-GGX) (G)
float smithGeometry(float NdotV, float NdotL, float roughness) {
    float a = max(roughness * roughness, EPSILON);
    float k = a * 0.5;
    float g1v = NdotV / max(NdotV * (1.0 - k) + k, EPSILON);
    float g1l = NdotL / max(NdotL * (1.0 - k) + k, EPSILON);
    return g1v * g1l;
}

// Fresnel (F)
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    float t = clamp(1.0 - cosTheta, 0.0, 1.0);
    float t2 = t * t;
    return F0 + (vec3(1.0) - F0) * (t2 * t2 * t);
}

// TODO: Recreate the implemtation and the base on how this is going to work
//
vec3 cookTorranceBdrfBase(vec3 normal, vec3 viewDirection, GPUMaterial objectMaterial, float u1, float u2) {
    // Sample GGX world (D)
    vec3 H = sampleGGXWorld(normal, objectMaterial.roughness, u1, u2);

    // Get NdotL to check for a ray not being visable
    vec3 bounceDirection = reflect(viewDirection, H);
    float NdotL = dot(normal, bounceDirection);
    if (NdotL <= 0.0) {
        return vec3(0.0);
    }

    // Values for the next BDRF calculations
    float NdotV = max(dot(normal, -viewDirection), EPSILON);
    float NdotH = max(dot(normal, H), EPSILON);
    float HdotV = max(dot(H, -viewDirection), EPSILON);

    // Get the G & F components
    float G = smithGeometry(NdotV, NdotL, objectMaterial.roughness);
    vec3 F = fresnelSchlick(HdotV, objectMaterial.albedo.xyz);

    // Update the color mult (BRDF * cos / PDF simplifies cleanly to this)
    return (F * G * HdotV) / (NdotV * NdotH);
}

vec3 cookTorranceBdrfBase(vec3 normal, vec3 viewDirection, vec3 microfacetNormal, GPUMaterial objectMaterial, float u1, float u2) {

    // Get NdotL to check for a ray not being visable
    vec3 bounceDirection = reflect(viewDirection, microfacetNormal);
    float NdotL = dot(normal, bounceDirection);
    if (NdotL <= 0.0) {
        return vec3(0.0);
    }

    // Values for the next BDRF calculations
    float NdotV = max(dot(normal, -viewDirection), EPSILON);
    float NdotH = max(dot(normal, microfacetNormal), EPSILON);
    float HdotV = max(dot(microfacetNormal, -viewDirection), EPSILON);

    // Get the G & F components
    float G = smithGeometry(NdotV, NdotL, objectMaterial.roughness);
    vec3 F = fresnelSchlick(HdotV, objectMaterial.albedo.xyz);

    // Update the color mult (BRDF * cos / PDF simplifies cleanly to this)
    return (F * G * HdotV) / (NdotV * NdotH);
}
