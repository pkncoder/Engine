#version 410 core

out vec4 FragColor;
in vec2 TexCoords;

// --- Samplers ---
uniform sampler2D u_GBuffer0; // Position
uniform sampler2D u_GBuffer1; // Normal
uniform sampler2D u_GBuffer2; // Albedo
uniform sampler2D u_GBuffer3; // RMA

uniform samplerCube u_ShadowMap;
uniform float u_FarPlane;

// --- UBOs ---
layout(std140) uniform CameraUBO {
    vec4 uCameraPos; 
    mat4 uViewProjection;
    mat4 uInverseView;
};

struct PointLightData {
    vec3 position;
    float padding; // Required for std140 alignment
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

float CalculateShadow(vec3 fragPos, vec3 lightPos, vec3 normal, vec3 lightDir) {
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    
    if (currentDepth < 0.0001 || currentDepth > u_FarPlane) 
        return 0.0;

    // 1. Normalize the light direction so offsets are purely angular
    vec3 dir = normalize(fragToLight);
    
    // 2. Dynamic Bias
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // Scales slightly based on distance to soften distant shadows
    float viewDistance = length(uCameraPos.xyz - fragPos);
    float diskRadius = (1.0 + (viewDistance / u_FarPlane)) / 250.0; // Reduced from 0.025 to ~0.004

    float shadow = 0.0;
    int samples = 20;

    for(int i = 0; i < samples; ++i) {
        vec3 sampleVector = dir + gridSamplingDisk[i] * diskRadius;
        float closestDepth = texture(u_ShadowMap, sampleVector).r * u_FarPlane;
        
        if(currentDepth - bias > closestDepth) {
            shadow += 1.0;
        }
    }

    return shadow / float(samples);
}

void main() {
    // 1. Unpack G-Buffer
    vec3 FragPos = texture(u_GBuffer0, TexCoords).xyz;
    vec3 Normal  = normalize(texture(u_GBuffer1, TexCoords).xyz);
    vec3 Albedo = texture(u_GBuffer2, TexCoords).rgb;
    
    // If the normal is 0, we are looking at the background (skybox area), just render black/clear color.
    if (length(Normal) < 0.1) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    // 2. Basic Lighting Setup
    vec3 viewDir = normalize(uCameraPos.xyz - FragPos);
    vec3 finalLighting = vec3(0.0);
    
    // Add a tiny bit of ambient light so completely shadowed areas aren't pitch black
    vec3 ambient = Albedo * 0.1; 
    finalLighting += ambient;

    // 3. Iterate over Point Lights
    int activeLights = pointLightCount;
    for(int i = 0; i < activeLights; ++i) {
        // Calculate light directions and distance
        vec3 lightDir = uPointLights[i].position - FragPos;
        float distance = length(lightDir);
        lightDir = normalize(lightDir);

        // Attenuation (Inverse square law)
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
        
        // Diffuse
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = diff * Albedo * uPointLights[i].emissive * uPointLights[i].intensity;
        
        // Specular (Blinn-Phong)
        vec3 halfwayDir = normalize(lightDir + viewDir + vec3(0.0001));
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 128.0); // 32.0 is the shininess factor
        vec3 specular = spec * uPointLights[i].emissive;

        // Apply attenuation
        diffuse *= attenuation;
        specular *= attenuation;

        float shadow = 0.0;
        if (i == 0) { 
            shadow = CalculateShadow(FragPos, uPointLights[i].position, Normal, lightDir);
        }

        // Apply the shadow (1.0 - shadow means 0.0 contribution when in shadow)
        vec3 lighting = (diffuse + specular) * (1.0 - shadow);
        finalLighting += lighting;
    }

    FragColor = vec4(finalLighting, 1.0);
}
