#version 460 core

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// --- Data Structures ---

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

layout(std430, binding = 0) readonly buffer VertexBuffer { GPUVertex vertices[]; };
layout(std430, binding = 1) readonly buffer IndexBuffer { uint indices[]; };
layout(std430, binding = 2) readonly buffer MeshEntryBuffer { GPUMeshEntry meshEntries[]; };
layout(std430, binding = 3) readonly buffer InstanceBuffer { GPUInstance instances[]; };

uniform int u_InstanceCount;
uniform vec3 u_cameraPos;
// Adding these to make the camera actually look around
uniform mat4 u_InverseView; 
uniform mat4 u_InverseProj;

layout(rgba32f, binding = 0) uniform image2D img_output;

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

// Möller–Trumbore Intersection
HitInfo rayTriangle(Ray ray, vec3 v0, vec3 v1, vec3 v2) {
    HitInfo hit;
    hit.hit = false;

    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;
    vec3 pvec = cross(ray.direction, edge2);
    float det = dot(edge1, pvec);

    // If determinant is near zero, ray is parallel to triangle
    if (abs(det) < 0.000001) return hit;
    
    float invDet = 1.0 / det;
    vec3 tvec = ray.origin - v0;
    float u = dot(tvec, pvec) * invDet;
    if (u < 0.0 || u > 1.0) return hit;

    vec3 qvec = cross(tvec, edge1);
    float v = dot(ray.direction, qvec) * invDet;
    if (v < 0.0 || u + v > 1.0) return hit;

    float t = dot(edge2, qvec) * invDet;

    if (t > 0.001) { // 0.001 is a small epsilon to prevent self-intersection
        hit.hit = true;
        hit.dist = t;
        hit.hitPos = ray.origin + ray.direction * t;
        hit.normal = normalize(cross(edge1, edge2));
    }

    return hit;
}

HitInfo rayScene(Ray ray) {
    HitInfo closestHit;
    closestHit.hit = false;
    closestHit.dist = 1e30; // Use a very large float

    for (int instID = 0; instID < u_InstanceCount; instID++) {
        GPUInstance instance = instances[instID];

        // 1. Transform ray to Local Space
        vec3 localOrigin = (instance.invTransform * vec4(ray.origin, 1.0)).xyz;
        vec3 localDir = (instance.invTransform * vec4(ray.direction, 0.0)).xyz;
        
        // IMPORTANT: We must normalize the local direction to keep distances accurate
        float rayLen = length(localDir);
        localDir /= rayLen;

        Ray localRay = Ray(localOrigin, localDir);
        GPUMeshEntry entry = meshEntries[instance.meshIndex];

        for (uint i = 0; i < entry.indexCount; i += 3) {
            uint gIdx0 = indices[entry.baseIndex + i + 0];
            uint gIdx1 = indices[entry.baseIndex + i + 1];
            uint gIdx2 = indices[entry.baseIndex + i + 2];

            vec3 v0 = vertices[entry.baseVertex + gIdx0].position.xyz;
            vec3 v1 = vertices[entry.baseVertex + gIdx1].position.xyz;
            vec3 v2 = vertices[entry.baseVertex + gIdx2].position.xyz;

            HitInfo currentHit = rayTriangle(localRay, v0, v1, v2);

            // Re-scale distance back to World Space
            currentHit.dist /= rayLen;

            if (currentHit.hit && currentHit.dist < closestHit.dist) {
                closestHit = currentHit;
                
                // Transform normal back to world space 
                // (Using transpose of inverse is technically better for non-uniform scale)
                closestHit.normal = normalize((instance.invTransform * vec4(currentHit.normal, 0.0)).xyz);
            }
        }
    }
    return closestHit;
}

void main() {
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 img_size = imageSize(img_output);

    if (pixel_coords.x >= img_size.x || pixel_coords.y >= img_size.y) return;

    // 1. Generate Screen-Space coordinates (-1 to 1)
    // Adding 0.5 centers the ray in the middle of the pixel
    vec2 ndc = (vec2(pixel_coords) + 0.5) / vec2(img_size) * 2.0 - 1.0;
    
    // 2. Adjust for Aspect Ratio
    float aspect = float(img_size.x) / float(img_size.y);
    float FOV = 60.0;
    float tanHalfFOV = tan(radians(FOV * 0.5));
    
    // 3. Create the ray direction in view space
    vec3 rayDirView = normalize(vec3(ndc.x * aspect * tanHalfFOV, ndc.y * tanHalfFOV, -1.0));

    // 4. Transform ray to World Space
    // Note: This assumes you pass u_InverseView from your C++ Camera
    vec3 rayDirWorld = (u_InverseView * vec4(rayDirView, 0.0)).xyz;

    Ray ray = Ray(u_cameraPos, normalize(rayDirWorld));

    // 5. Trace the scene
    HitInfo hit = rayScene(ray);

    vec3 col;
    if (hit.hit) {
        // Simple "NdotL" shading based on a fake light source
        float light = max(dot(hit.normal, normalize(u_cameraPos)), 0.1);
        col = vec3(0.8, 0.3, 0.2) * light; // Reddish color
    } else {
        // Sky Gradient
        float t = 0.5 * (ray.direction.y + 1.0);
        col = (1.0 - t) * vec3(1.0) + t * vec3(0.5, 0.7, 1.0);
    }

    imageStore(img_output, pixel_coords, vec4(hit.hit ? vec3(hit.hitPos) : vec3(0.0), 1.0));
}
