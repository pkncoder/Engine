#version 330 core

#include "../../include/common.glsl"
#include "../../include/sharedStructures.glsl"

#include "../uniforms.glsl"

#include "../models/blinnPhong.glsl"

void main() {

    if (length(u_emmissive) > 0.01) {
        FragColor = vec4(u_emmissive, 1.0);
        return;
    }

    vec3 baseColor;
    if (u_hasAlbedoMap == 1) {
        // Sample texture and multiply by base albedo (allows tinting!)
        vec4 texColor = texture(u_albedoMap, v_TexCoords);
        baseColor = texColor.rgb; 
    } else {
        baseColor = u_albedo;
    }

    vec3 color = blinnPhong(u_viewPos, v_worldPos, v_normal, Material(baseColor, u_emmissive, u_roughness, u_metallic), u_viewPos, Material(vec3(0.0), vec3(1.0), 0.0, 0.0));

    FragColor = vec4(color, 1.0);

}
