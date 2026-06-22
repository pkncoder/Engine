// ============================================================================
// SHARED PBR MATHEMATICAL PRIMITIVES
// ============================================================================

// GGX Microfacet distribution function (D)
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

// Microfacet Masking-Shadowing Function (Smith-GGX) (G)
// Remaps 'k' differently for analytical direct lights vs stochastic sampled rays
float GeometrySchlickGGX(float NdotV, float roughness, bool isDirectLight) {
    float r = roughness;
    float k;
    if (isDirectLight) {
        k = ((r + 1.0) * (r + 1.0)) / 8.0; // Rasterizer remapping
    } else {
        k = (r * r) / 2.0;                 // Path Tracer / IBL remapping
    }

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / max(denom, EPSILON);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness, bool isDirectLight) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness, isDirectLight);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness, isDirectLight);

    return ggx2 * ggx1;
}

// Fresnel (F)
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (vec3(1.0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Stochastic GGX Microfacet Sampling for Path Tracing
vec3 sampleGGXWorld(vec3 normal, float roughness, float u1, float u2) {
    float a = roughness * roughness;
    float phi = TWO_PI * u1;

    float cosTheta = sqrt(max((1.0 - u2) / max(1.0 + (a * a - 1.0) * u2, EPSILON), 0.0));
    float sinTheta = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));

    vec3 localH = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
    return getBasis(normal) * localH;
}

// ============================================================================
// OVERLOAD 1: RASTERIZER / ANALYTICAL DIRECT LIGHTING MODEL
// ============================================================================
vec3 cookTorranceBDRFBase(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in Material lightMaterial) {
    vec3 N = normalize(normal);
    vec3 V = normalize(viewPos - worldPos);
    vec3 L = normalize(lightPos - worldPos);
    vec3 H = normalize(V + L);

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, objectMaterial.albedo.rgb, objectMaterial.metallic);

    float D = DistributionGGX(N, H, objectMaterial.roughness);
    float G = GeometrySmith(N, V, L, objectMaterial.roughness, true); // True = Direct analytical lighting
    vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = D * G * F;
    float denominator = 4.0 * NdotV * NdotL;
    vec3 specular = numerator / max(denominator, EPSILON);

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - objectMaterial.metallic);

    vec3 lightColor = lightMaterial.emissive.rgb; 
    vec3 diffuse = objectMaterial.albedo.rgb / PI;
    vec3 ambient = 0.03 * objectMaterial.albedo.rgb;

    return ambient + (kD * diffuse + specular) * lightColor * NdotL;
}

// ============================================================================
// OVERLOAD 2: PATH TRACER STOCHASTIC RAY GENERATION (BRDF Importance Sampling)
// ============================================================================
// Takes random uniforms, generates a bounce direction, returns attenuation weight
vec3 cookTorranceBDRFBase(const in vec3 normal, const in vec3 viewDirection, const in Material objectMaterial, float u1, float u2, out vec3 outBounceDirection) {
    vec3 N = normalize(normal);
    vec3 V = normalize(-viewDirection); // Ensure V points outward away from the surface

    // Sample the microfacet normal (H) using GGX distribution
    vec3 H = sampleGGXWorld(N, objectMaterial.roughness, u1, u2);
    
    // Reflect incoming ray about the microfacet to get the outgoing light ray
    outBounceDirection = reflect(viewDirection, H); 
    vec3 L = normalize(outBounceDirection);

    float NdotL = dot(N, L);
    if (NdotL <= 0.0) {
        return vec3(0.0); // Absorbed/Reflected into surface
    }

    float NdotV = max(dot(N, V), EPSILON);
    float NdotH = max(dot(N, H), EPSILON);
    float HdotV = max(dot(H, V), EPSILON);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, objectMaterial.albedo.rgb, objectMaterial.metallic);

    float G = GeometrySmith(N, V, L, objectMaterial.roughness, false); // False = Path traced indirect ray
    vec3 F  = fresnelSchlick(HdotV, F0);

    // Simplified weight multiplier: (BRDF * cos(theta)) / PDF 
    // Strips away 'D' and geometry division factors cleanly
    vec3 specularWeight = (F * G * HdotV) / (NdotV * NdotH);
    
    return specularWeight;
}

// // ============================================================================
// // OVERLOAD 3: PATH TRACER MIS EVALUATION (Direct Light Sampling / Next Event Estimation)
// // ============================================================================
// // Evaluates the precise raw BRDF value and PDF for a given explicit target direction
// vec3 cookTorranceBDRFBase(const in vec3 normal, const in vec3 viewDirection, const in vec3 explicitLightDirection, const in Material objectMaterial, out float outPdf) {
//     vec3 N = normalize(normal);
//     vec3 V = normalize(-viewDirection); // Ensure V points outward
//     vec3 L = normalize(explicitLightDirection);
//     vec3 H = normalize(V + L);
//
//     float NdotL = max(dot(N, L), 0.0);
//     float NdotV = max(dot(N, V), 0.0);
//
//     if (NdotL <= 0.0 || NdotV <= 0.0) {
//         outPdf = 0.0;
//         return vec3(0.0);
//     }
//
//     float NdotH = max(dot(N, H), EPSILON);
//     float HdotV = max(dot(H, V), EPSILON);
//
//     vec3 F0 = vec3(0.04);
//     F0 = mix(F0, objectMaterial.albedo, objectMaterial.metallic);
//
//     float D = DistributionGGX(N, H, objectMaterial.roughness);
//     float G = GeometrySmith(N, V, L, objectMaterial.roughness, false);
//     vec3 F  = fresnelSchlick(HdotV, F0);
//
//     // Calculate structural sampling probability density function for MIS weight balancing
//     outPdf = (D * NdotH) / (4.0 * HdotV);
//
//     // Evaluate standard raw PBR components
//     vec3 specular = (D * G * F) / max(4.0 * NdotV * NdotL, EPSILON);
//     vec3 kS = F;
//     vec3 kD = (vec3(1.0) - kS) * (1.0 - objectMaterial.metallic);
//     vec3 diffuse = objectMaterial.albedo / PI;
//
//     return (kD * diffuse) + specular;
// }
