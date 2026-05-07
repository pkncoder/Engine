#version 460 core

// Compute shader blocks
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Common
// BEGIN INCLUDE: ../include/common.glsl
#define EPSILON 0.0001
#define PI 3.14159265359
#define REALLY_FAR 999999999.0
// END INCLUDE: ../include/common.glsl

// Shared includes
// BEGIN INCLUDE: ../include/sharedStructures.glsl
struct Ray {
    vec3 origin;
    vec3 direction;
};

struct HitInfo {
    bool hit;
    float dist;
    vec3 hitPos;
    vec3 normal;
};
// END INCLUDE: ../include/sharedStructures.glsl
// BEGIN INCLUDE: ../include/sharedUniforms.glsl
// Camera position + inverseView matrix
uniform vec3 u_cameraPos;
uniform mat4 u_inverseView;

// Camera FOV
uniform float u_FOV;
// END INCLUDE: ../include/sharedUniforms.glsl
// BEGIN INCLUDE: ../include/intersections.glsl
HitInfo rayTriangle(Ray ray, vec3 v0, vec3 v1, vec3 v2) {
    HitInfo hit;
    hit.hit = false;

    const vec3 edge1 = v1 - v0;
    const vec3 edge2 = v2 - v0;
    const vec3 pvec = cross(ray.direction, edge2);
    const float det = dot(edge1, pvec);

    if (abs(det) < EPSILON) return hit;

    const float invDet = 1.0 / det;
    const vec3 tvec = ray.origin - v0;
    const float u = dot(tvec, pvec) * invDet;
    if (u < 0.0 || u > 1.0) return hit;

    const vec3 qvec = cross(tvec, edge1);
    const float v = dot(ray.direction, qvec) * invDet;
    if (v < 0.0 || u + v > 1.0) return hit;

    const float t = dot(edge2, qvec) * invDet;

    if (t > EPSILON) {
        hit.hit = true;
        hit.dist = t;
        hit.hitPos = ray.origin + ray.direction * t;
        hit.normal = normalize(cross(edge1, edge2));
    }

    return hit;
}

// END INCLUDE: ../include/intersections.glsl

// Coloring models
// BEGIN INCLUDE: ../include/models/blinn_phong.glsl
vec3 blinnPhong(Ray ray, HitInfo hit) {

    // Light position is just the camera at the moment
    const vec3 lightPos = u_cameraPos;

    // Basic Properties
    const vec3 lightColor = vec3(1.0);
    const vec3 matColor = vec3(0.4, 0.8, 0.5);
    const vec3 normal = hit.normal;

    // Ambient
    const vec3 ambient = 0.15 * lightColor * matColor;

    // Diffuse
    const vec3 lightDir = normalize(lightPos - hit.hitPos);
    const float diffuseStrength = max(dot(normal, lightDir), 0.0);
    const vec3 diffuse = lightColor * matColor * diffuseStrength;

    // Get the view & halfway vectors
    const vec3 viewDir = normalize(u_cameraPos - hit.hitPos);
    const vec3 halfwayDir = normalize(lightDir + viewDir); 

    // Specular (Blinn-Phong)
    const float specularStrength = pow(max(dot(normal, halfwayDir), 0.0), 16.0); 
    const float specularPower = 0.5;
    const vec3 specular = lightColor * specularPower * specularStrength; 

    // Final color
    const vec3 result = ambient + diffuse + specular;

    // Return the result
    return result;
}

// END INCLUDE: ../include/models/blinn_phong.glsl

// Path tracing includes
// BEGIN INCLUDE: ../include/pathTracingStructures.glsl
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
    uint padding1;
    uint padding2;
    uint padding3;
};

// END INCLUDE: ../include/pathTracingStructures.glsl

// SSBO buffers
layout(std430, binding = 0) readonly buffer MeshEntryBuffer { GPUMeshEntry meshEntries[]; };
layout(std430, binding = 1) readonly buffer VertexBuffer { GPUVertex vertices[]; };
layout(std430, binding = 2) readonly buffer IndexBuffer { uint indices[]; };
layout(std430, binding = 3) readonly buffer InstanceBuffer { GPUInstance instances[]; };

// Total instance count
uniform int u_instanceCount;

// Final image writeout
layout(rgba32f, binding = 0) uniform image2D img_output;

HitInfo rayScene(Ray ray) {

    // Create a closestHit with temp values meant to be overwritten
    HitInfo closestHit;
    closestHit.hit = false;
    closestHit.dist = REALLY_FAR;

    // Loop every instance
    for (int instID = 0; instID < u_instanceCount; instID++) {

        // Get the current instance
        const GPUInstance instance = instances[instID];

        // Get a new ray in local space
        vec3 localRayOrigin = (instance.invTransform * vec4(ray.origin, 1.0)).xyz;
        vec3 localRayDirection = normalize((instance.invTransform * vec4(ray.direction, 0.0)).xyz);
       
        // Construct the ray and save it's length
        const Ray localRay = Ray(localRayOrigin, localRayDirection);
        const float rayLength = length(localRayDirection);

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

            // Re-scale distance back to World Space
            currentHit.dist /= rayLength;

            // See if it's closer or not
            if (currentHit.hit && currentHit.dist < closestHit.dist) {

                // If it is, set this as the closest hit
                closestHit = currentHit;
                
                // Transform hitPos & normal back into world space
                closestHit.hitPos = (instance.transform * vec4(currentHit.hitPos, 1.0)).xyz;
                closestHit.normal = normalize((vec4(currentHit.normal, 0.0) * instance.invTransform).xyz);
            }
        }
    }

    // Return the closest hit found
    return closestHit;
}

void main() {

    // Calculate pixel uv
    const ivec2  pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    const ivec2 img_size = imageSize(img_output);

    // Error check on image size
    if (pixel_coords.x >= img_size.x || pixel_coords.y >= img_size.y) return;

    // Get screen space coords
    const vec2 uv = (vec2(pixel_coords) + 0.5) / vec2(img_size) * 2.0 - 1.0;
    
    // Get the aspect ratio
    const float aspectRatio = float(img_size.x) / float(img_size.y);
    const float tanHalfFOV = tan(radians(u_FOV * 0.5));
    
    // Get the ray direction that's in view space
    const vec3 rayDirView = normalize(vec3(uv, -1.0) * vec3(aspectRatio * tanHalfFOV, tanHalfFOV, 1.0));

    // Move the ray direction in view space into world space
    const vec3 rayDirWorld = (u_inverseView * vec4(rayDirView, 0.0)).xyz;

    // Construct the final ray
    const Ray ray = Ray(u_cameraPos, normalize(rayDirWorld));

    // Find the hit in the scene
    const HitInfo hit = rayScene(ray);

    // Color based on the hit
    vec3 col;
    if (hit.hit) {
        // Phong-shade to get the color
        col = blinnPhong(ray, hit);
    } else {
        // Sky Gradient
        const float t = 0.5 * (ray.direction.y + 1.0);
        col = (1.0 - t) * vec3(1.0) + t * vec3(0.5, 0.7, 1.0);
    }

    // Write to the image with the final hit color
    imageStore(img_output, pixel_coords, vec4(col, 1.0));
}
