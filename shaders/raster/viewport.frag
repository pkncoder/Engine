#version 330 core

in vec3 v_normal;
in vec3 v_worldPos;

out vec4 FragColor;

uniform vec3 u_viewPos;   // The Camera's position in world space

uniform vec3 u_albedo;
uniform vec3 u_emmissive;
uniform float u_roughness;
uniform float u_metallic;

#include "../include/sharedStructures.glsl"
#include "../include/models/blinn_phong.glsl"

void main() {

    vec3 color = blinnPhong(u_viewPos, v_worldPos, v_normal, Material(u_albedo, u_emmissive, u_roughness, u_metallic), u_viewPos, Material(vec3(0.0), vec3(1.0), 0.0, 0.0));

    FragColor = vec4(color, 1.0);

}
