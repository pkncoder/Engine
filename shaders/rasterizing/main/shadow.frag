#version 330 core

in vec3 vFragWorldPos;

uniform vec3  uLightPos;
uniform float uFarPlane;

uniform sampler2D uAlphaMap;
uniform float     uAlphaCutoff;

out float fragDepth;
in vec2 vTexCoords; // add this to shadow.vert too (pass through from aTexCoords)

void main() {
    // Discard before writing depth — cutout geometry gets correct shadow shape
    if (texture(uAlphaMap, vTexCoords).r < uAlphaCutoff)
        discard;

    fragDepth = length(vFragWorldPos - uLightPos) / uFarPlane;
}
