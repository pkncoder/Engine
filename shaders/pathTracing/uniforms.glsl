uniform int uFrameNum;

// Final image writeout
layout(rgba32f, binding = 0) uniform image2D MainColorOutput;
layout(rgba32f, binding = 1) uniform image2D Normals;
layout(rgba32f, binding = 2) uniform image2D Albedo;
layout(rgba32f, binding = 3) uniform image2D Emissive;
layout(rgba32f, binding = 4) uniform image2D IMR;
layout(rgba32f, binding = 5) uniform image2D Depth;
layout(rgba32f, binding = 6) uniform image2D Hit;
