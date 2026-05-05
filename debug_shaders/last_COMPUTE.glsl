// #version 430 core
#version 330 core

// 1. Define the local group size (8x8 threads per block)
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// 2. The output texture (binding must match your glBindTextureUnit/glBindImageTexture)
layout(rgba32f, binding = 0) uniform image2D img_output;

void main() {
    // Get the integer coordinates of the current pixel
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    
    // Get the dimensions of the output texture
    ivec2 img_size = imageSize(img_output);

    // Prevent out-of-bounds writing if the texture size isn't a multiple of 8
    if (pixel_coords.x >= img_size.x || pixel_coords.y >= img_size.y) {
        return;
    }

    // 3. Calculate Normalized UV coordinates (0.0 to 1.0)
    // We cast to float to ensure precision
    vec2 uv = vec2(pixel_coords) / vec2(img_size);

    // 4. Create a color based on UV
    // Red = U (Horizontal), Green = V (Vertical), Blue = 0, Alpha = 1
    vec4 pixel_color = vec4(uv.x, uv.y, 0.0, 1.0);

    // 5. Write the color to the texture
    imageStore(img_output, pixel_coords, pixel_color);
}
