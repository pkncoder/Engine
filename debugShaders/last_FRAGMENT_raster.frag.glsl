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

// TODO: Merge
struct Material {
    vec3 albedo;
    vec3 emmisive;

    float roughness;
    float metallic;
};

struct GPUMaterial {
    vec4 albedo;
    vec4 emissive;
    float roughness;
    float metallic;
};
// END INCLUDE: ../../include/sharedStructures.glsl
// BEGIN INCLUDE: ../uniforms.glsl
in vec3 vNormal;
in vec3 vWorldPos;
in vec2 vTexCoords;

out vec4 FragColor;

uniform vec3 uViewPos;   // The Camera's position in world space

uniform vec3 uAlbedo;
uniform sampler2D uAlbedoMap;
uniform int uHasAlbedoMap;

uniform vec3 uEmmissive;
uniform float uRoughness;
uniform float uMetallic;
// END INCLUDE: ../uniforms.glsl

// BEGIN INCLUDE: ../models/blinnPhong.glsl
// BEGIN INCLUDE: ../../include/modelBases/blinnPhongBase.glsl
// TODO: Double-check the implementation of this
vec3 blinnPhongBase(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in  Material lightMaterial) {

    // Basic Properties
    vec3 lightColor = lightMaterial.emmisive;
    vec3 objectColor = objectMaterial.albedo;

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
vec3 cookTorranceBDRFBase(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in Material lightMaterial) {
    vec3 N = normalize(normal);
    vec3 V = normalize(viewPos - worldPos);
    vec3 L = normalize(lightPos - worldPos);
    vec3 H = normalize(V + L);

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    // Calculate base reflectivity F0 (Dielectrics use ~0.04 baseline, metals use their Albedo)
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, objectMaterial.albedo, objectMaterial.metallic);

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
    vec3 lightColor = lightMaterial.emmisive; // Using emitter parameters as light source color
    vec3 diffuse = objectMaterial.albedo / PI;

    // Simple default global Ambient baseline
    vec3 ambient = 0.03 * objectMaterial.albedo;

    return ambient + (kD * diffuse + specular) * lightColor * NdotL;
}
// END INCLUDE: ../../include/modelBases/cookTorranceBDRFBase.glsl

// TODO: Fix/learn how to use
vec3 cookTorranceBDRF(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in Material lightMaterial) {
    return cookTorranceBDRFBase(viewPos, worldPos, normal, objectMaterial, lightPos, lightMaterial);
}

// END INCLUDE: ../models/cookTorranceBDRF.glsl

uniform sampler2D uEmissiveMap;
uniform sampler2D uNormalMap;
uniform sampler2D uRoughnessMap; // NEW: Roughness map sampler
uniform sampler2D uMetallicMap;  // NEW: Metallic map sampler
uniform int uIsBumpMap;

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
    // 1. Compute World Space Normal Map / Bump Map
    mat3 TBN = calculateTBN(normalize(vNormal), vWorldPos, vTexCoords);

    vec3 localNormal;
    if (uIsBumpMap == 1) {
        localNormal = bumpToTangentNormal(uNormalMap, vTexCoords, vWorldPos, 0.02);
    } else {
        localNormal = normalize(texture(uNormalMap, vTexCoords).rgb * 2.0 - 1.0);
    }

    vec3 worldNormal = normalize(TBN * localNormal);

    // 2. Sample and modulate PBR Map Values
    vec3 albedo = texture(uAlbedoMap, vTexCoords).rgb * uAlbedo;
    vec3 emissive = texture(uEmissiveMap, vTexCoords).rgb * uEmmissive;
    float roughness = texture(uRoughnessMap, vTexCoords).r * uRoughness;
    float metallic = texture(uMetallicMap, vTexCoords).r * uMetallic;

    // 3. Assemble our Material Structure
    Material mat = Material(albedo, emissive, roughness, metallic);
    
    // Setting up a dummy light right at the camera's location ("Headlight setup")
    Material lightMat = Material(vec3(0.0), vec3(1.0) * 3.0, 0.0, 0.0); 

    // 4. Evaluate lighting equations
    vec3 color = cookTorranceBDRF(uViewPos, vWorldPos, worldNormal, mat, uViewPos, lightMat);
    // vec3 color = blinnPhong(uViewPos, vWorldPos, worldNormal, mat, uViewPos, lightMat);

    // 5. Add Emissive Glow on top of the calculated lighting
    color += emissive;

    // Output final fragment color
    FragColor = vec4(color, 1.0);
}
