#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uLightSpaceMatrix;

out vec3 vFragWorldPos;

void main() {
    vec4 worldPos     = uModel * vec4(aPos, 1.0);
    vFragWorldPos     = worldPos.xyz;
    gl_Position       = uLightSpaceMatrix * worldPos;
}
