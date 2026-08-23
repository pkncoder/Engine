#version 410 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out; // 3 vertices * 6 faces = 18

uniform mat4 uShadowMatrices[6]; // The 6 view-projection matrices

out vec4 FragPos; // Passed to the fragment shader

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
