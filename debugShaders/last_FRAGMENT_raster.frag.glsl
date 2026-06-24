#version 330 core

// BEGIN INCLUDE: ../../include/common.glsl
// Numbers
#define PI 3.14159265359
#define TWO_PI 6.28318530718
#define INV_PI 0.31830988618
#define INV_TWO_PI  0.15915494309
#define EPSILON 0.001
#define FAR 25.0
#define REALLY_FAR 50.0
#define SUPER_FAR 90.0

// Get the local basis matrix
// TODO: move
mat3 getBasis(vec3 n) {
    vec3 up = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
    vec3 t = normalize(cross(up, n));
    vec3 b = cross(n, t);
    return mat3(t, b, n);
}
// END INCLUDE: ../../include/common.glsl
// BEGIN INCLUDE: ../../include/sharedStructures.glsl
struct Ray {
    vec3 origin;
    vec3 direction;
    vec3 invDirection;
};

struct HitInfo {
    bool hit;
    float dist;

    vec3 hitPos;
    vec3 normal;

    int objectIndex;
    uint materialIndex;
};

struct Material {
    vec4 albedo;
    vec4 emissive;
    float roughness;
    float metallic;
};
// END INCLUDE: ../../include/sharedStructures.glsl
// BEGIN INCLUDE: ../../include/sharedUniforms.glsl
// Camera position + inverseView matrix
uniform vec3 uCameraPos;
uniform mat4 uInverseView;

// Camera FOV
uniform float uFOV;
// END INCLUDE: ../../include/sharedUniforms.glsl

// BEGIN INCLUDE: ../uniforms.glsl

// Values from the vertex shader
in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vTexCoords;

// Final color output
out vec4 FragColor;

// Base material values
uniform vec3 uAlbedo;
uniform vec3 uEmmissive;
uniform float uRoughness;
uniform float uMetallic;

// Texture material values
uniform sampler2D uAlbedoMap;
uniform sampler2D uEmissiveMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uMetallicMap;

// Normal map + uniform for bump map detection
uniform sampler2D uNormalMap;
uniform int uIsBumpMap;
// END INCLUDE: ../uniforms.glsl

// BEGIN INCLUDE: ../models/blinnPhong.glsl
// BEGIN INCLUDE: ../../include/modelBases/blinnPhongBase.glsl
vec3 blinnPhongBase(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in Material lightMaterial) {

    // Basic Properties
    vec3 lightColor = lightMaterial.emissive.rgb;
    vec3 objectColor = objectMaterial.albedo.rgb;

    // Ambient
    vec3 ambient = 0.15 * lightColor * objectColor;

    // Diffuse
    vec3 lightDir = normalize(lightPos - worldPos);
    float nDotL = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * objectColor * nDotL;

    // Get the view & halfway vectors
    vec3 viewDir = normalize(viewPos - worldPos);
    vec3 halfwayDir = normalize(lightDir + viewDir); 

    // Specular (Blinn-Phong)
    float shininess = max(1.0, (1.0 - objectMaterial.roughness) * 256.0);
    float specularStrength = pow(max(dot(normal, halfwayDir), 0.0), shininess); 

    // Scale specular strength down as it gets rougher
    float specularPower = 1.0 - objectMaterial.roughness;
    vec3 specular = lightColor * specularPower * specularStrength; 

    // Final color
    vec3 result = ambient + diffuse + specular;

    // Return the result
    return result;
}
// END INCLUDE: ../../include/modelBases/blinnPhongBase.glsl

vec3 blinnPhong(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in  Material lightMaterial) {
    // Pass in the math
    return blinnPhongBase(viewPos, worldPos, normal, objectMaterial, lightPos, lightMaterial);
}
// END INCLUDE: ../models/blinnPhong.glsl
// BEGIN INCLUDE: ../models/cookTorranceBDRF.glsl
// BEGIN INCLUDE: ../../include/modelBases/cookTorranceBDRFBase.glsl
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
// END INCLUDE: ../../include/modelBases/cookTorranceBDRFBase.glsl

vec3 cookTorranceBDRF(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in Material lightMaterial) {
    return cookTorranceBDRFBase(viewPos, worldPos, normal, objectMaterial, lightPos, lightMaterial);
}

// END INCLUDE: ../models/cookTorranceBDRF.glsl

mat3 calculateTBN(vec3 N, vec3 p, vec2 uv) {
    vec3 dp1 = dFdx(p);
    vec3 dp2 = dFdy(p);
    vec2 duv1 = dFdx(uv);
    vec2 duv2 = dFdy(uv);

    vec3 dp2perp = cross(dp2, N);
    vec3 dp1perp = cross(N, dp1);
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    // --- FIXED: Secure against division-by-zero / NaN ---
    float denom = max(dot(T, T), dot(B, B));
    if (denom < EPSILON) {
        // Fallback: Create a clean procedural basis if the face has broken UVs
        vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
        T = normalize(cross(up, N));
        B = cross(N, T);
    } else {
        float invmax = inversesqrt(denom);
        T *= invmax;
        B *= invmax;
    }

    return mat3(T, B, N);
}

vec3 bumpToTangentNormal(sampler2D heightMap, vec2 uv, vec3 worldPos, float heightAmplitude) {
    vec2 texelSize = 1.0 / vec2(textureSize(heightMap, 0));

    float hL = textureLod(heightMap, uv - vec2(texelSize.x, 0.0), 0.0).r;
    float hR = textureLod(heightMap, uv + vec2(texelSize.x, 0.0), 0.0).r;
    float hD = textureLod(heightMap, uv - vec2(0.0, texelSize.y), 0.0).r;
    float hU = textureLod(heightMap, uv + vec2(0.0, texelSize.y), 0.0).r;

    float worldPerUV = length(dFdx(worldPos)) / max(length(dFdx(uv)), EPSILON);
    float worldTexelSize = worldPerUV * texelSize.x;

    float dxWorld = (hR - hL) * heightAmplitude;
    float dyWorld = (hU - hD) * heightAmplitude;

    float slopeX = dxWorld / (2.0 * worldTexelSize);
    float slopeY = dyWorld / (2.0 * worldTexelSize);

    return normalize(vec3(-slopeX, -slopeY, 1.0));
}

void main() {

    // Get the world space normalizer matrix
    mat3 TBN = calculateTBN(normalize(vNormal), vWorldPos, vTexCoords);

    // Find the local normal through the texture
    vec3 localNormal;
    if (uIsBumpMap == 1) {
        localNormal = bumpToTangentNormal(uNormalMap, vTexCoords, vWorldPos, 0.02);
    } else {
        localNormal = normalize(texture(uNormalMap, vTexCoords).rgb * 2.0 - 1.0);
    }

    // Calculate the world normal
    vec3 worldNormal = normalize(TBN * localNormal);

    // Sample & get the material values
    vec3 albedo = texture(uAlbedoMap, vTexCoords).rgb * uAlbedo;
    vec3 emissive = texture(uEmissiveMap, vTexCoords).rgb * uEmmissive;
    float roughness = texture(uRoughnessMap, vTexCoords).r * uRoughness;
    float metallic = texture(uMetallicMap, vTexCoords).r * uMetallic;

    if (length(emissive) > 0.0) {
        FragColor = vec4(emissive, 0.0);
        return;
    }

    // Get the material struct
    Material mat = Material(vec4(albedo, 0.0), vec4(emissive, 0.0), roughness, metallic);

    // Get the dummy-light material
    Material lightMat = Material(vec4(0.0), vec4(1.0) * 1.0, 0.0, 0.0); 

    // Get the light color
    // vec3 color = cookTorranceBDRF(uCameraPos, vWorldPos, worldNormal, mat, uCameraPos, lightMat);
    vec3 color = blinnPhong(uCameraPos, vWorldPos, worldNormal, mat, uCameraPos, lightMat);

    // Add the emissive glow to the color
    color += emissive;

    // Add an ambient amount
    color += albedo * 0.1;

    // Output final fragment color
    FragColor = vec4(color, 1.0);
}
