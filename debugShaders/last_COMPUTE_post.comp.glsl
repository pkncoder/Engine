#version 460 core

// Compute shader blocks
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// BEGIN INCLUDE: ../../include/utils/srgb.glsl
vec3 vecLessThan(vec3 f, float value) {
    return vec3(
        (f.x < value) ? 1.0f : 0.0f,
        (f.y < value) ? 1.0f : 0.0f,
        (f.z < value) ? 1.0f : 0.0f
    );
}

vec3 linearToSRGB(vec3 rgb) {
    rgb = clamp(rgb, 0.0f, 1.0f);
    return mix(
        pow(rgb, vec3(1.0f / 2.4f)) * 1.055f - 0.055f,
        rgb * 12.92f,
        vecLessThan(rgb, 0.0031308f)
    );
}

vec3 SRGBToLinear(vec3 rgb) {
    rgb = clamp(rgb, 0.0f, 1.0f);
    return mix(
        pow(((rgb + 0.055f) / 1.055f), vec3(2.4f)),
        rgb / 12.92f,
        vecLessThan(rgb, 0.04045f)
    );
}
// END INCLUDE: ../../include/utils/srgb.glsl
// BEGIN INCLUDE: ../../include/utils/toneMapping.glsl
vec3 ACESFilm(vec3 x) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x*(a*x + b)) / (x*(c*x + d) + e), 0.0f, 1.0f);
}
// END INCLUDE: ../../include/utils/toneMapping.glsl

// Path-tracer specific uniforms
// BEGIN INCLUDE: ../uniforms.glsl
uniform uint uFrameNum;

// Final image writeout
layout(rgba32f, binding = 0) uniform image2D MainColorOutput;
layout(rgba32f, binding = 1) uniform image2D PostProcessedOutput;
layout(rgba32f, binding = 2) uniform image2D Normal;
layout(rgba32f, binding = 3) uniform image2D Albedo;
layout(rgba32f, binding = 4) uniform image2D Emissive;
layout(rgba32f, binding = 5) uniform image2D IMR;
layout(rgba32f, binding = 6) uniform image2D Depth;
layout(rgba32f, binding = 7) uniform image2D Hit;
// END INCLUDE: ../uniforms.glsl

void main() {

   // Calculate pixel uv
   const ivec2  pixelCoords = ivec2(gl_GlobalInvocationID.xy);
   const ivec2 imgSize = imageSize(PostProcessedOutput);

   // Error check on image size
   if (pixelCoords.x >= imgSize.x || pixelCoords.y >= imgSize.y) return;

   vec3 pixelColor = imageLoad(MainColorOutput, pixelCoords).rgb;
   vec3 col = linearToSRGB(ACESFilm(pixelColor));

   imageStore(PostProcessedOutput, pixelCoords, vec4(col, 1.0));
}
