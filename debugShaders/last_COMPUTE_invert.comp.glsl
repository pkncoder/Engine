#version 460 core 

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D img_output;

void main() {

    // Calculate pixel uv
    const ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    const ivec2 img_size = imageSize(img_output);

    vec3 col = imageLoad(img_output, pixel_coords).xyz;

    imageStore(img_output, pixel_coords, vec4(1.0 - col, 1.0));
}
