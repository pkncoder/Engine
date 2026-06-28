#version 330 core

in vec3 vFragWorldPos;

uniform vec3  uLightPos;
uniform float uFarPlane;

out float fragDepth; // writes into the GL_R32F color attachment

void main() {
    fragDepth = length(vFragWorldPos - uLightPos) / uFarPlane;
}
