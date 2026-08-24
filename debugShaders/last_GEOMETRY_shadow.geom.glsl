#version 410 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out; // 3 vertices * 6 faces = 18

out vec4 FragPos; // Passed to the fragment shader

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
    for(int face = 0; face < 6; ++face) {
        gl_Layer = face; // Built-in variable routing to the cubemap face
        
        for(int i = 0; i < 3; ++i) { // For each vertex of the triangle
            FragPos = gl_in[i].gl_Position;
            gl_Position = uShadowMatrices[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive(); // Finish the triangle for this face
    }
}
