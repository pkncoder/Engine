#version 410 core

layout(location = 0) out vec4 outPosition; 
layout(location = 1) out vec4 outNormal;   
layout(location = 2) out vec4 outAlbedo;   
layout(location = 3) out vec4 outRMA;      

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Material Uniforms (Make sure to pass these in your C++ execute loop!)
uniform vec3 u_Albedo;
uniform float u_Roughness;
uniform float u_Metallic;

void main() {
    // 1. Position
    outPosition = vec4(FragPos, 1.0);
    
    // 2. Normal (Normalized to fix interpolation artifacts)
    outNormal = vec4(normalize(Normal), 1.0);
    
    // 3. Albedo (Using the uniform, falling back to a debug color if not set)
    // If you haven't set the uniform yet, this will likely default to black. 
    // You can hardcode vec3(0.8) here temporarily if you want to skip setting uniforms right now.
    outAlbedo = vec4(u_Albedo, 1.0); 
    
    // 4. RMA (Roughness, Metallic, AO)
    outRMA = vec4(u_Roughness, u_Metallic, 1.0, 1.0); 
}
