#version 330 core

#include "../../include/common.glsl"
#include "../../include/sharedStructures.glsl"

#include "../uniforms.glsl"

#include "../models/blinnPhong.glsl"

void main() {

    if (length(uEmmissive) > 0.01) {
        FragColor = vec4(uEmmissive, 1.0);
        return;
    }

    vec3 baseColor;
    if (uHasAlbedoMap == 1) {
        vec4 texColor = texture(uAlbedoMap, vTexCoords);
        baseColor = texColor.rgb * uAlbedo;
    } else {
        baseColor = uAlbedo;
    }

    vec3 color = blinnPhong(uViewPos, vWorldPos, vNormal, Material(baseColor, uEmmissive, uRoughness, uMetallic), uViewPos, Material(vec3(0.0), vec3(1.0), 0.0, 0.0));

    FragColor = vec4(color, 1.0);

}
