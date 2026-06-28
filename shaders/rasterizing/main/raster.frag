#version 330 core

#include "../../include/common.glsl"
#include "../../include/sharedStructures.glsl"
#include "../../include/sharedUniforms.glsl"
#include "../../include/utils/srgb.glsl"
#include "../../include/utils/toneMapping.glsl"

#include "../uniforms.glsl"

#include "../models/blinnPhong.glsl"
#include "../models/cookTorranceBDRF.glsl"

// TODO: move
#define EXPOSURE 0.12
#define FOG_END 20.0

uniform samplerCube uShadowCubeMap;
uniform float       uShadowFarPlane;
uniform vec3      uLightPos;

uniform vec2 uResolution; // Pass your window/viewport width and height here
const float uVignetteRadius = 1.5; // Controls the size of the clear center area (lower = larger vignette)
const float uVignetteSoftness = 0.9; // Controls the softness of the fade transition

const vec3  uFogColor   = vec3(0.3, 0.3, 0.3); // Change to your desired fog color (e.g., light gray/blue)
const float uFogDensity = 0.02;

// Add worldPos parameter
float calcShadow(vec3 worldPos, vec3 normal) {
    vec3  fragToLight = worldPos - uLightPos;
    float currentDist = length(fragToLight) / uShadowFarPlane;

    vec3  lightDir = normalize(-fragToLight);
    float bias     = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);

    // 20-tap 3D PCF — offsets spread around a sphere for smooth penumbra
    vec3 sampleOffsets[20] = vec3[](
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );

    float shadow     = 0.0;
    float diskRadius = 0.05; // tune for harder/softer edges
    for (int i = 0; i < 20; i++) {
        float closestDist = texture(uShadowCubeMap,
                                    fragToLight + sampleOffsets[i] * diskRadius).r;
        shadow += (currentDist - bias > closestDist) ? 1.0 : 0.0;
    }
    return shadow / 20.0;
}

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

    float shadow = calcShadow(vWorldPos, worldNormal);

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
    vec3 color = blinnPhong(uCameraPos, vWorldPos, worldNormal, mat, uLightPos, lightMat, shadow);

    // Add the emissive glow to the color
    color += emissive;

    color *= EXPOSURE;
    color = (ACESFilm(color));

    float fogDist = length(uCameraPos - vWorldPos);
    float fogFactor = exp(-pow(fogDist * uFogDensity, 2.0));
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    color = mix(uFogColor, color, fogFactor);

    vec2 uv = gl_FragCoord.xy / uResolution;
    vec2 centerCoord = uv * 2.0 - 1.0;
    float centerDist = length(centerCoord);
    float vignette = smoothstep(uVignetteRadius, uVignetteRadius - uVignetteSoftness, centerDist);

    color *= vignette;

    // Output final fragment color
    FragColor = vec4(color, 1.0);
}
