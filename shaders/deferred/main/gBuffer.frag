#version 410 core

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outAlbedo;
layout(location = 3) out vec4 outRMA;
layout(location = 4) out vec4 outEmissive;


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
    
    // If the normal map is flat (close to 0,0,1 in tangent space), evaluate the bump map
    if (length(tangentNormal.xy) < 0.01) {
        vec2 texelSize = 1.0 / vec2(textureSize(u_BumpMap, 0));
        
        // Sample current pixel and right/down neighbors
        float h0 = texture(u_BumpMap, TexCoords).r;
        float hx = texture(u_BumpMap, TexCoords + vec2(texelSize.x, 0.0)).r;
        float hy = texture(u_BumpMap, TexCoords + vec2(0.0, texelSize.y)).r;
        
        // Calculate tangent normal from height differences
        float bumpStrength = 5.0; // Adjust this value to make the bump more/less intense
        tangentNormal = normalize(vec3((h0 - hx) * bumpStrength, (h0 - hy) * bumpStrength, 1.0));
    }
    
    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    float det = st1.s * st2.t - st2.s * st1.t;
    if (abs(det) < 1e-8) {
        return normalize(Normal);
    }
    
    vec3 N = normalize(Normal);
    vec3 T = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
void main() {
    // 1. Alpha Clipping
    float alpha = texture(u_AlphaMap, TexCoords).r;
    if (alpha < 0.1) { // TODO: This is specifically for masks
        discard; // Throw away this pixel completely if it's transparent
    }

    outPosition = vec4(FragPos, 1.0);
    
    // 2. Normal / Bump
    // You can blend bump mapping logic here later, but for now we apply the Normal map
    outNormal = vec4(getNormalFromMap(), 1.0);
    
    // 3. Albedo
    vec4 texColor = texture(u_AlbedoMap, TexCoords);
    outAlbedo = vec4(texColor.rgb * u_AlbedoColor, alpha); 

    if (length(u_EmissiveColor) > 0.09) {
      outAlbedo.xyz *= u_EmissiveColor;
    }

    vec3 u_EmissiveTextureColor = texture(u_EmissiveMap, TexCoords).rgb;
    if (length(u_EmissiveTextureColor) > 0.09) {
      outAlbedo.xyz *= u_EmissiveTextureColor;
    }
    
    // 4. RMA (Roughness, Metallic, AO)
    outRMA = vec4(u_Roughness, u_Metallic, 1.0, 1.0); 
    
    vec3 emissiveTexColor = texture(u_EmissiveMap, TexCoords).rgb;
    outEmissive = vec4(emissiveTexColor, 1.0);
}
