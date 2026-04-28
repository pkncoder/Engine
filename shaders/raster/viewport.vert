#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec3 vNormal;
out vec3 vWorldPos; // Renamed for clarity

void main() {
    // 1. Transform vertex to World Space
    vWorldPos = vec3(u_Model * vec4(aPos, 1.0));

    // 2. Transform Normal to World Space 
    // We use mat3 to ignore translation. 
    // Ideally, pass 'transpose(inverse(u_Model))' from C++ for non-uniform scale support.
    vNormal = normalize(mat3(u_Model) * aNormal);

    // 3. Final clip-space position
    gl_Position = u_ViewProjection * vec4(vWorldPos, 1.0);
}
