#version 410 core

out vec4 FragColor;

in vec2 TexCoords;

// The samplers mapped in lightingPass.execute[cite: 10]
uniform sampler2D u_GBuffer0; // Position
uniform sampler2D u_GBuffer1; // Normal
uniform sampler2D u_GBuffer2; // Albedo
uniform sampler2D u_GBuffer3; // RMA

void main() {
    // ----------------------------------------------------
    // DEBUG OUTPUT:
    // Simply render the screen UVs to the final output quad
    // You should see a gradient: Green at the top, Red on the right, Yellow top-right
    // ----------------------------------------------------
    
    FragColor = vec4(TexCoords, 0.0, 1.0);
}
