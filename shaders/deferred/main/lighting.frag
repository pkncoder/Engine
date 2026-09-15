#version 410 core

out vec4 FragColor;
in vec2 TexCoords;

// --- Samplers ---
uniform sampler2D u_GBuffer0; // Position
uniform sampler2D u_GBuffer1; // Normal
uniform sampler2D u_GBuffer2; // Albedo
uniform sampler2D u_GBuffer3; // RMA

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
    float pointLightCount;
};

void main() {
    // 1. Unpack G-Buffer
    vec3 FragPos = texture(u_GBuffer0, TexCoords).xyz;
    vec3 Normal = texture(u_GBuffer1, TexCoords).xyz;
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
    vec3 ambient = Albedo * 0.05; 
    finalLighting += ambient;

    // 3. Iterate over Point Lights
    int activeLights = int(pointLightCount);
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
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0); // 32.0 is the shininess factor
        vec3 specular = spec * uPointLights[i].emissive * uPointLights[i].intensity;

        // Apply attenuation
        diffuse *= attenuation;
        specular *= attenuation;

        finalLighting += (diffuse + specular);
    }

    FragColor = vec4(finalLighting, 1.0);
}
