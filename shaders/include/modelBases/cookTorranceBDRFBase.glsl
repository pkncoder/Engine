float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / max(denom, EPSILON);
}

// Geometry Function (Smith's Schlick-GGX for Analytical Lights)
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0; // Direct light remapping factor

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / max(denom, EPSILON);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx2 * ggx1;
}

// Fresnel Function (Schlick)
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (vec3(1.0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Fixed Rasterizer analytical PBR baseline
// TODO: Rework?
vec3 cookTorranceBDRFBase(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in Material lightMaterial) {
    vec3 N = normalize(normal);
    vec3 V = normalize(viewPos - worldPos);
    vec3 L = normalize(lightPos - worldPos);
    vec3 H = normalize(V + L);

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    // Calculate base reflectivity F0 (Dielectrics use ~0.04 baseline, metals use their Albedo)
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, objectMaterial.albedo.rgb, objectMaterial.metallic);

    // Evaluate Cook-Torrance Microfacet Loop
    float D = DistributionGGX(N, H, objectMaterial.roughness);
    float G = GeometrySmith(N, V, L, objectMaterial.roughness);
    vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

    // Specular Term
    vec3 numerator = D * G * F;
    float denominator = 4.0 * NdotV * NdotL;
    vec3 specular = numerator / max(denominator, EPSILON);

    // Energy Conservation: Diffuse vs Specular
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - objectMaterial.metallic; // Metallic surfaces have zero diffuse light refraction

    // Final analytical light accumulation
    vec3 lightColor = lightMaterial.emissive.rgb; // Using emitter parameters as light source color
    vec3 diffuse = objectMaterial.albedo.rgb / PI;

    // Simple default global Ambient baseline
    vec3 ambient = 0.03 * objectMaterial.albedo.rgb;

    return ambient + (kD * diffuse + specular) * lightColor * NdotL;
}
