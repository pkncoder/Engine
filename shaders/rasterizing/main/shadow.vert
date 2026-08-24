#version 410 core
layout (location = 0) in vec3 aPos;

out vec4 FragPos;

layout(std140) uniform GlobalSceneUBO {
    mat4 uShadowMatrices[6];
    vec4 uLightPos;
    vec2 uResolution;
    float uFOV;
    float uShadowFarPlane;
};

layout(std140) uniform ObjectUBO {
    mat4 uModel;
    vec4 uAlbedo;     // Ensure vec4 in GLSL
    vec4 uEmissive;   // Ensure vec4 in GLSL
    float uRoughness;
    float uMetallic;
    int uIsBumpMap;
};

void main() {
    // Just pass the world-space position to the Geometry Shader
    FragPos = uModel * vec4(aPos, 1.0);
    gl_Position = FragPos;
}
