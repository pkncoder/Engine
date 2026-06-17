#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 u_viewProjection;
uniform mat4 u_model;

out vec3 v_normal;
out vec3 v_worldPos; // Renamed for clarity
out vec2 v_TexCoords;

void main() {
    // 1. Transform vertex to World Space
    v_worldPos = vec3(u_model * vec4(aPos, 1.0));

    // 2. Transform Normal to World Space 
    // We use mat3 to ignore translation. 
    // Ideally, pass 'transpose(inverse(u_Model))' from C++ for non-uniform scale support.
    v_normal = normalize(mat3(u_model) * aNormal);
    v_TexCoords = aTexCoords;

    // 3. Final clip-space position
    gl_Position = u_viewProjection * vec4(v_worldPos, 1.0);
}
