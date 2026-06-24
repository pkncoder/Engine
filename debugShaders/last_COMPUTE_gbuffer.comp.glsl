#version 460 core

// Compute shader blocks
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Common
// BEGIN INCLUDE: ../../include/common.glsl
// Numbers
#define PI 3.14159265359
#define TWO_PI 6.28318530718
#define INV_PI 0.31830988618
#define INV_TWO_PI  0.15915494309
#define EPSILON 0.001
#define FAR 25.0
#define REALLY_FAR 50.0
#define SUPER_FAR 90.0

// Get the local basis matrix
// TODO: move
mat3 getBasis(vec3 n) {
    vec3 up = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
    vec3 t = normalize(cross(up, n));
    vec3 b = cross(n, t);
    return mat3(t, b, n);
}
// END INCLUDE: ../../include/common.glsl

// Shared includes
// BEGIN INCLUDE: ../../include/sharedStructures.glsl
struct Ray {
    vec3 origin;
    vec3 direction;
    vec3 invDirection;
};

struct HitInfo {
    bool hit;
    float dist;

    vec3 hitPos;
    vec3 normal;

    // TODO: this is stupid
    int objectIndex;
    uint materialIndex;
};

struct Material {
    vec4 albedo;
    vec4 emissive;
    float roughness;
    float metallic;
};
// END INCLUDE: ../../include/sharedStructures.glsl
// BEGIN INCLUDE: ../../include/sharedUniforms.glsl
// Camera position + inverseView matrix
uniform vec3 uCameraPos;
uniform mat4 uInverseView;

// Camera FOV
uniform float uFOV;
// END INCLUDE: ../../include/sharedUniforms.glsl

// Coloring models

// Path tracing includes
// BEGIN INCLUDE: ../structures.glsl
struct GPUMeshEntry {
    uint baseVertex;
    uint baseIndex;
    uint indexCount;
    uint padding;
};

struct GPUVertex {
    vec4 position;
    vec4 normal;
    vec4 texCoords;
};

struct GPUInstance {
    mat4 transform;
    mat4 invTransform;
    uint meshIndex;
    uint materialIndex;
    uint padding2;
    uint padding3;
};
// END INCLUDE: ../structures.glsl
// BEGIN INCLUDE: ../buffers.glsl
// SSBO buffers
layout(std430, binding = 0) readonly buffer MeshEntryBuffer { GPUMeshEntry meshEntries[]; };
layout(std430, binding = 1) readonly buffer VertexBuffer { GPUVertex vertices[]; };
layout(std430, binding = 2) readonly buffer IndexBuffer { uint indices[]; };
layout(std430, binding = 3) readonly buffer InstanceBuffer { GPUInstance instances[]; };
layout(std430, binding = 4) readonly buffer MaterialBuffer { Material materials[]; };

// Total instance count
uniform int uInstanceCount;
// END INCLUDE: ../buffers.glsl

// Path tracing intersection functions
// BEGIN INCLUDE: ../intersections.glsl
HitInfo rayTriangle(Ray ray, vec3 v0, vec3 v1, vec3 v2) {
    HitInfo hit;
    hit.hit = false;

    const vec3 edge1 = v1 - v0;
    const vec3 edge2 = v2 - v0;
    const vec3 pvec = cross(ray.direction, edge2);
    const float det = dot(edge1, pvec);

    if (abs(det) < EPSILON) return hit; // Parallel check is fine here

    const float invDet = 1.0 / det;
    const vec3 tvec = ray.origin - v0;
    const float u = dot(tvec, pvec) * invDet;
    if (u < 0.0 || u > 1.0) return hit;

    const vec3 qvec = cross(tvec, edge1);
    const float v = dot(ray.direction, qvec) * invDet;
    if (v < 0.0 || u + v > 1.0) return hit;

    const float t = dot(edge2, qvec) * invDet;

    // Use a tiny math threshold here, NOT the global shadow EPSILON
    if (t > 0.000001) { 
        hit.hit = true;
        hit.dist = t;
        // hitPos removed from here; it's better calculated in world space later
        hit.normal = normalize(cross(edge1, edge2));
    }

    return hit;
}
HitInfo rayScene(Ray ray) {

    // Create a closestHit with temp values meant to be overwritten
    HitInfo closestHit;
    closestHit.hit = false;
    closestHit.dist = REALLY_FAR;

    // Loop every instance
    for (int instID = 0; instID < uInstanceCount; instID++) {

        // Get the current instance
        const GPUInstance instance = instances[instID];

        // Get a new ray in local space
        vec3 localRayOrigin = (instance.invTransform * vec4(ray.origin, 1.0)).xyz;
        vec3 localRayDirection = (instance.invTransform * vec4(ray.direction, 0.0)).xyz;
        const float rayLength = length(localRayDirection);

        // Normalize localRayDirection
        localRayDirection /= rayLength;
       
        // Construct the ray and save it's length
        const Ray localRay = Ray(localRayOrigin, localRayDirection, 1.0 / localRayDirection);

        // Get the current mesh entry
        const GPUMeshEntry entry = meshEntries[instance.meshIndex];

        // Loop each index, skip by three (three verts per triangle)
        for (uint i = 0; i < entry.indexCount; i += 3) {

            // Get each index
            const uint idx0 = indices[entry.baseIndex + i + 0];
            const uint idx1 = indices[entry.baseIndex + i + 1];
            const uint idx2 = indices[entry.baseIndex + i + 2];

            // Get each vertex
            const vec3 v0 = vertices[entry.baseVertex + idx0].position.xyz;
            const vec3 v1 = vertices[entry.baseVertex + idx1].position.xyz;
            const vec3 v2 = vertices[entry.baseVertex + idx2].position.xyz;

            // Test the ray-triangle intersection for this triangle
            HitInfo currentHit = rayTriangle(localRay, v0, v1, v2);
            currentHit.objectIndex = instID;
            currentHit.materialIndex = instID;

            // Re-scale distance back to World Space BEFORE checking EPSILON
            currentHit.dist /= rayLength;

            // Apply the global EPSILON check here in uniform World Space
            if (currentHit.hit && currentHit.dist > EPSILON && currentHit.dist < closestHit.dist) {

                // If it is, set this as the closest hit
                closestHit = currentHit;

                // FIX 1: Calculate perfectly aligned World hitPos using the ray
                closestHit.hitPos = ray.origin + ray.direction * closestHit.dist;
                
                // Transform normal back into world space
                closestHit.normal = normalize((vec4(currentHit.normal, 0.0) * instance.invTransform).xyz);
                
                // FIX 3: Force the normal to always face the incoming ray to prevent trapped bounces
                if (dot(closestHit.normal, ray.direction) > 0.0) {
                    closestHit.normal = -closestHit.normal;
                }
            }
        }
    }

    // Return the closest hit found
    return closestHit;
}

// END INCLUDE: ../intersections.glsl

// Path-tracer specific uniforms
// BEGIN INCLUDE: ../uniforms.glsl
uniform int uFrameNum;

// Final image writeout
layout(rgba32f, binding = 0) uniform image2D MainColorOutput;
layout(rgba32f, binding = 1) uniform image2D Normals;
layout(rgba32f, binding = 2) uniform image2D Albedo;
layout(rgba32f, binding = 3) uniform image2D Emissive;
layout(rgba32f, binding = 4) uniform image2D IMR;
layout(rgba32f, binding = 5) uniform image2D Depth;
layout(rgba32f, binding = 6) uniform image2D Hit;
// END INCLUDE: ../uniforms.glsl

/* ----------------- COLORING FUNCTIONS ----------------- */

void main() {

    // Calculate pixel uv
    const ivec2  pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    const ivec2 imgSize = imageSize(MainColorOutput);

    // Error check on image size
    if (pixelCoords.x >= imgSize.x || pixelCoords.y >= imgSize.y) return;

    // Get screen space coords
    const vec2 uv = (vec2(pixelCoords) + 0.5) / vec2(imgSize) * 2.0 - 1.0;

    // Get the aspect ratio
    const float aspectRatio = float(imgSize.x) / float(imgSize.y);

    // Get the ray direction that's in view space and put it into world space
    const float tanHalfFOV = tan(radians(uFOV * 0.5)); // Helper value
    const vec3 rayDirView = normalize(vec3(uv, -1.0) * vec3(aspectRatio * tanHalfFOV, tanHalfFOV, 1.0));
    const vec3 rayDirWorld = normalize((uInverseView * vec4(rayDirView, 0.0)).xyz);

    // Construct the final ray
    const Ray ray = Ray(uCameraPos, rayDirWorld, 1.0 / rayDirWorld);

    // Find the hit in the scene
    const HitInfo hit = rayScene(ray);

    if (!hit.hit) {

        // Hit image data
        imageStore(Normals, pixelCoords, vec4(0.0));
        imageStore(Depth, pixelCoords, vec4(0.0));
        imageStore(Hit, pixelCoords, vec4(0.0));

        // Material hit image data
        imageStore(Albedo, pixelCoords, vec4(0.0));
        imageStore(Emissive, pixelCoords, vec4(0.0));
        imageStore(IMR, pixelCoords, vec4(0.0));

        return;
    }

    const Material material = materials[hit.materialIndex];

    // Hit image data
    // TODO: Figure out how to deal with debug visulizations in things like normal & depth
    imageStore(Hit, pixelCoords, vec4(hit.materialIndex, hit.materialIndex, 0.0, hit.hit));
    imageStore(Depth, pixelCoords, vec4(vec3(hit.dist), 1.0));
    imageStore(Normals, pixelCoords, vec4(hit.normal, 1.0));

    // Material hit image data
    imageStore(Albedo, pixelCoords, vec4(material.albedo.rgb, 1.0));
    imageStore(Emissive, pixelCoords, vec4(material.emissive.rgb, 1.0));
    imageStore(IMR, pixelCoords, vec4(0.0, material.metallic, material.roughness, 1.0));

}

