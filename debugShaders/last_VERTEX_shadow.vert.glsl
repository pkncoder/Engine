#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 uModel;
uniform mat4 uLightSpaceMatrix;

out vec3 vFragWorldPos;
out vec2 vTexCoords;

// In main():
void main() {
    vec4 worldPos     = uModel * vec4(aPos, 1.0);
    vFragWorldPos     = worldPos.xyz;
    vTexCoords = aTexCoords;

    gl_Position       = uLightSpaceMatrix * worldPos;
}
