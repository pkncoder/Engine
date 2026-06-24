#version 460 core 

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D MainColorOutput;

void main() {

    // Calculate pixel uv
    const ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    const ivec2 imgSize = imageSize(MainColorOutput);

    vec3 col = imageLoad(MainColorOutput, pixelCoords).xyz;

    imageStore(MainColorOutput, pixelCoords, vec4(1.0 - col, 1.0));
}
