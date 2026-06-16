#version 330 core

#include "../include/common.glsl"

#include "../include/raster/uniforms.glsl"
#include "../include/sharedStructures.glsl"

#include "../include/raster/models/blinn_phong.glsl"

void main() {

    if (length(u_emmissive) > 0.01) {
        FragColor = vec4(u_emmissive, 1.0);
        return;
    }

    vec3 color = blinnPhong(u_viewPos, v_worldPos, v_normal, Material(u_albedo, u_emmissive, u_roughness, u_metallic), u_viewPos, Material(vec3(0.0), vec3(1.0), 0.0, 0.0));

    FragColor = vec4(color, 1.0);

}
