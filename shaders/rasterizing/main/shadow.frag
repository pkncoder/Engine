#version 410 core
in vec4 FragPos;

uniform vec3 uLightPos;
uniform float uFarPlane;

void main() {
    // Calculate distance between fragment and light source
    float lightDistance = length(FragPos.xyz - uLightPos);
    
    // Map to [0, 1] range by dividing by far_plane
    lightDistance = lightDistance / uFarPlane;
    
    // Write this as the depth value
    gl_FragDepth = lightDistance;
}
