#version 330 core
in vec4 FragPos;

uniform vec3 u_LightPos;
uniform float u_FarPlane;

void main() {
    float lightDistance = length(FragPos.xyz - u_LightPos);
    
    // Map to [0; 1] range by dividing by far_plane
    lightDistance = lightDistance / u_FarPlane;
    gl_FragDepth = clamp(lightDistance, 0.0, 1.0);}
