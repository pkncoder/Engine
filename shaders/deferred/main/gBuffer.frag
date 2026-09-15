#version 410 core

// These locations route directly to the MRTs you set up in IRenderer
layout(location = 0) out vec4 outPosition; // GL_RGBA32F
layout(location = 1) out vec4 outNormal;   // GL_RGBA16F
layout(location = 2) out vec4 outAlbedo;   // GL_RGBA8
layout(location = 3) out vec4 outRMA;      // GL_RGBA8

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main() {
    outPosition = vec4(FragPos, 1.0);
    outNormal = vec4(normalize(Normal), 1.0);
    
    // Writing mesh UVs to Albedo just in case you want to debug the G-Buffer directly
    outAlbedo = vec4(TexCoords, 0.0, 1.0); 
    outRMA = vec4(1.0, 0.0, 0.0, 1.0); // Default rough/non-metallic
}
