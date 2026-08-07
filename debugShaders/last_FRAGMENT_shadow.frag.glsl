#version 330 core

in vec3 vFragWorldPos;

uniform vec3  uLightPos;
uniform float uFarPlane;

uniform sampler2D uAlphaMap;
const float alphaCuttoff = 0.2;

out float fragDepth;
in vec2 vTexCoords; // add this to shadow.vert too (pass through from aTexCoords)

void main() {
    // Discard before writing depth — cutout geometry gets correct shadow shape
    if (texture(uAlphaMap, vTexCoords).r < alphaCuttoff)
        discard;

    fragDepth = length(vFragWorldPos - uLightPos) / uFarPlane;
}
