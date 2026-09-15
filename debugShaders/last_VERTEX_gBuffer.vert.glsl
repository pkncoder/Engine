#version 410 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

// std140 ensures memory alignment matches your C++ CameraData struct
layout(std140) uniform CameraUBO {
    vec4 uCameraPos; // vec4 to ensure 16-byte alignment
    mat4 uViewProjection;
    mat4 uInverseView;
};

uniform mat4 u_Model;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main() {
    vec4 worldPos = u_Model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    
    // Calculate normal matrix to handle non-uniform scaling
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;
    TexCoords = aTexCoords;
    
    gl_Position = uViewProjection * worldPos;
}
