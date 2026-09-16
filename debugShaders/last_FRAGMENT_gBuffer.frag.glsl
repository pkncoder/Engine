#version 410 core

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outAlbedo;
layout(location = 3) out vec4 outRMA;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// --- Texture Samplers ---
uniform sampler2D u_AlbedoMap;
uniform sampler2D u_EmissiveMap;
uniform sampler2D u_AlphaMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_BumpMap;

// --- Material Uniforms ---
uniform vec3 u_AlbedoColor;
uniform vec3 u_EmissiveColor;
uniform float u_Roughness;
uniform float u_Metallic;

// Helper to calculate tangent space using screen-space derivatives
vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(u_NormalMap, TexCoords).xyz * 2.0 - 1.0;
    
    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T   = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B   = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main() {
    // 1. Alpha Clipping
    float alpha = texture(u_AlphaMap, TexCoords).r;
    if (alpha < 0.1) {
        discard; // Throw away this pixel completely if it's transparent
    }

    outPosition = vec4(FragPos, 1.0);
    
    // 2. Normal / Bump
    // You can blend bump mapping logic here later, but for now we apply the Normal map
    outNormal = vec4(getNormalFromMap(), 1.0);
    
    // 3. Albedo
    vec4 texColor = texture(u_AlbedoMap, TexCoords);
    outAlbedo = vec4(texColor.rgb * u_AlbedoColor, 1.0); 
    
    // 4. RMA (Roughness, Metallic, AO)
    outRMA = vec4(u_Roughness, u_Metallic, 1.0, 1.0); 
    
    // A quick note on Emissive: 
    // We are binding the Emissive map, but your G-Buffer does not currently have 
    // a 5th render target to store Emissive data! 
}
