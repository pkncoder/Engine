#version 330 core

#include "../../include/common.glsl"
#include "../../include/sharedStructures.glsl"
#include "../uniforms.glsl"

#include "../models/blinnPhong.glsl"
#include "../models/cookTorranceBDRF.glsl"

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
