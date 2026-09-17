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

float CalculateShadow(vec3 fragPos, vec3 lightPos, vec3 normal, vec3 lightDir) {
    vec3 fragToLight = fragPos - lightPos;
    float closestDepth = texture(u_ShadowMap, fragToLight).r;
    closestDepth *= u_FarPlane;
    float currentDepth = length(fragToLight);
    
    // DYNAMIC BIAS: Scales between 0.05 and 0.25 based on the angle to the light
    float bias = max(0.25 * (1.0 - dot(normal, lightDir)), 0.05); 
    
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

vec3 getNormalFromMap() {
    // 1. Calculate the base normal from the Normal Map
    vec3 tangentNormal = texture(u_NormalMap, TexCoords).xyz * 2.0 - 1.0;
    
    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    float det = st1.s * st2.t - st2.s * st1.t;
    vec3 N = normalize(Normal);
    
    vec3 mappedNormal = N;
    if (abs(det) >= 0.00001) {
        vec3 T = normalize(Q1*st2.t - Q2*st1.t);
        vec3 B = -normalize(cross(N, T));
        mat3 TBN = mat3(T, B, N);
        mappedNormal = normalize(TBN * tangentNormal);
    }
    
    // 2. Perturb the normal using the Bump (Height) Map
    float height = texture(u_BumpMap, TexCoords).r;
    
    // Calculate the slope of the height map
    float dHx = dFdx(height);
    float dHy = dFdy(height);
    
    // Tweak this value to make the bump effect stronger or weaker
    float bumpScale = 2.0; 
    
    // Apply the bump gradient to the normal
    vec3 bumpGradient = sign(det) * (dHx * cross(Q2, mappedNormal) + dHy * cross(mappedNormal, Q1));
    
    return normalize(mappedNormal - bumpGradient * bumpScale);
}

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
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0); // 32.0 is the shininess factor
        vec3 specular = spec * uPointLights[i].emissive * uPointLights[i].intensity;

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
