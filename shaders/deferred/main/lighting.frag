#version 330 core

#include "../../include/utils/srgb.glsl"
#include "../../include/utils/toneMapping.glsl"

#define SKYBOXMULT 1.0

out vec4 FragColor;
in vec2 TexCoords;

// G-Buffer Inputs
uniform sampler2D u_GBuffer0; // Position (RGB)
uniform sampler2D u_GBuffer1; // Normal (RGB)
uniform sampler2D u_GBuffer2; // Albedo (RGB)
uniform sampler2D u_GBuffer3; // RMA (R = Roughness, M = Metallic, A = AO)
uniform sampler2D u_GBuffer4; // Emissive

// Shadow Maps
uniform samplerCube u_ShadowMaps[4];
uniform int u_MaxShadowLights;
uniform float u_FarPlane;

// Camera Data
layout(std140) uniform CameraUBO {
    vec4 uCameraPos; 
    mat4 uViewProjection;
    mat4 uInverseView;
};

struct PointLightData {
    vec3 position;
    int padding; // Required for std140 alignment
    vec3 emissive; // Your struct uses emissive for light color
    float intensity;
};

layout(std140) uniform PointLightUBO {
    PointLightData uPointLights[4]; // Matches your hardcoded max of 4
    int pointLightCount;
};

// 20 pre-calculated offset directions distributed on a 3D sphere
const vec3 gridSamplingDisk[20] = vec3[](
   vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
   vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
   vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
   vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

// Helper function to safely sample array of samplers with constant indices
float SampleShadowCube(int idx, vec3 direction) {

    if (idx == 0) return texture(u_ShadowMaps[0], direction).r;
    else if (idx == 1) return texture(u_ShadowMaps[1], direction).r;
    else if (idx == 2) return texture(u_ShadowMaps[2], direction).r;
    else if (idx == 3) return texture(u_ShadowMaps[3], direction).r;
    
    return 1.0;
}

// ----------------------------------------------------------------------------
// Shadow Calculation
// ----------------------------------------------------------------------------
float CalculateShadow(vec3 fragPos, vec3 lightPos, int shadowIndex, vec3 normal, vec3 lightDir) {
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    if (currentDepth < 0.0001 || currentDepth > u_FarPlane) 
        return 1.0;

    vec3 dir = normalize(fragToLight + vec3(0.001));

    // Dynamic Bias
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // Scales slightly based on distance to soften distant shadows
    float viewDistance = length(uCameraPos.xyz - fragPos);
    float diskRadius = (1.0 + (viewDistance / u_FarPlane)) / 250.0; // Reduced from 0.025 to ~0.004

    float shadow = 0.0;
    int samples = 20;

    for(int i = 0; i < samples; ++i) {
        vec3 sampleVector = dir + gridSamplingDisk[i] * diskRadius;
        float closestDepth = SampleShadowCube(shadowIndex, normalize(sampleVector)) * u_FarPlane;

        if(currentDepth - bias > closestDepth) {
            shadow += 1.0;
        }
    }

    return shadow / float(samples);
}

// ----------------------------------------------------------------------------
// Main Lighting Pass
// ----------------------------------------------------------------------------
void main() {

    vec3 FragPos = texture(u_GBuffer0, TexCoords).xyz;
    vec3 Normal  = normalize(texture(u_GBuffer1, TexCoords).xyz);
    vec3 Albedo = texture(u_GBuffer2, TexCoords).rgb;

    // If the normal is 0, we are looking at the background (skybox area), just render black/clear color.
    if (length(Normal) < 0.1) {
        FragColor = vec4(vec3(0.0), 1.0);
        return;
    }

    // 2. Basic Lighting Setup
    vec3 viewDir = normalize(uCameraPos.xyz - FragPos);
    vec3 finalLighting = vec3(0.0);
    
    // Add a tiny bit of ambient light so completely shadowed areas aren't pitch black
    vec3 ambient = Albedo * 0.1; 
    finalLighting += ambient;

    // 2. Process active point lights
    for(int i = 0; i < pointLightCount; i++) {

        vec3 lightPos = uPointLights[i].position;
        vec3 lightColor = uPointLights[i].emissive * uPointLights[i].intensity;

        vec3 lightDir = normalize(lightPos - FragPos);
        float distance = length(lightDir);
        lightDir = normalize(lightDir);

        vec3 halfwayDir = normalize(lightDir + viewDir + vec3(0.0001));
        
        // Attenuation (Inverse square law)
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
                // Diffuse
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = diff * Albedo * uPointLights[i].emissive * uPointLights[i].intensity;
        
        // Specular (Blinn-Phong)
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 128.0); // 32.0 is the shininess factor
        vec3 specular = spec * uPointLights[i].emissive * 0.5;


        // Apply attenuation
        diffuse *= attenuation;
        specular *= attenuation;

        // Shadow lookup
        float shadow = CalculateShadow(FragPos, lightPos, i, Normal, lightDir);

        finalLighting += (diffuse + specular) * (1.0 - shadow);
    }

    vec3 emissive = texture(u_GBuffer4, TexCoords).rgb;

    // 4. Final output
    FragColor = vec4(finalLighting + emissive, 1.0);
    return;
}
