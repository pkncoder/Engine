#version 410 core

in vec4 FragPos;


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
    // Calculate distance between fragment and light source
    float lightDistance = length(FragPos.xyz - uLightPos.xyz);
    
    // Map to [0, 1] range by dividing by far_plane
    lightDistance = lightDistance / uShadowFarPlane;
    
    // Write this as the depth value
    gl_FragDepth = lightDistance;
}
