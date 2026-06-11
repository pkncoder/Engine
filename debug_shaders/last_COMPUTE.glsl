#version 460 core

// Compute shader blocks
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Common
// BEGIN INCLUDE: ../include/common.glsl
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
mat3 getBasis(vec3 n) {
    vec3 up = abs(n.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(0.0, 1.0, 0.0);
    vec3 t = normalize(cross(up, n));
    vec3 b = cross(n, t);
    return mat3(t, b, n);
}
// END INCLUDE: ../include/common.glsl
// BEGIN INCLUDE: ../include/random.glsl
uint seed; // Global seed

// Backup seed for when the blue noise texture isn't loaded
void setSeed(vec2 fragCoord, uint frameNum)
{
    seed = uint(fragCoord.x) * 1973u + uint(fragCoord.y) * 9277u + frameNum * 26699u | 1u;
}

// Hash function for random uint
uint wang_hash(inout uint seed) {
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}

// Random float one from [0-1]
float rnd1(inout uint seed){
    return float(wang_hash(seed)) / float(-1u);
}

// Random normalized unit vector
vec3 rndUnit(inout uint seed) {
    float z = rnd1(seed) * 2.0f - 1.0f;
    float a = rnd1(seed) * 6.28318530718;
    float r = sqrt(1.0f - z * z);
    return vec3(r * cos(a), r * sin(a), z);
}
// END INCLUDE: ../include/random.glsl

// Shared includes
// BEGIN INCLUDE: ../include/sharedStructures.glsl
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

    int objectIndex;
};

struct Material {
    vec3 albedo;
    vec3 emmisive;

    float roughness;
    float metallic;
};
// END INCLUDE: ../include/sharedStructures.glsl
// BEGIN INCLUDE: ../include/sharedUniforms.glsl
// Camera position + inverseView matrix
uniform vec3 u_cameraPos;
uniform mat4 u_inverseView;

// Camera FOV
uniform float u_FOV;
// END INCLUDE: ../include/sharedUniforms.glsl

// Coloring models
// BEGIN INCLUDE: ../include/models/blinn_phong.glsl
// TODO: Abstract the math and have the math be done in a seperate function
vec3 blinnPhong(const in Ray ray, const in HitInfo hit, const in Material objectMaterial, const in vec3 lightPos, const in Material lightMaterial) {

    // Basic Properties
    const vec3 lightColor = lightMaterial.emmisive;
    const vec3 objectColor = objectMaterial.albedo;
    const vec3 normal = hit.normal;

    // Ambient
    const vec3 ambient = 0.15 * lightColor * objectColor;

    // Diffuse
    const vec3 lightDir = normalize(lightPos - hit.hitPos);
    const float nDotL = max(dot(normal, lightDir), 0.0);
    const vec3 diffuse = lightColor * objectColor * nDotL;

    // Get the view & halfway vectors
    const vec3 viewDir = normalize(lightPos - hit.hitPos);
    const vec3 halfwayDir = normalize(lightDir + viewDir); 

    // Specular (Blinn-Phong)
    const float specularStrength = pow(max(dot(normal, halfwayDir), 0.0), 16.0); 
    const float specularPower = abs(objectMaterial.roughness - 1.0); // TODO: Fix
    const vec3 specular = lightColor * specularPower * specularStrength; 

    // Final color
    const vec3 result = ambient + diffuse + specular;

    // Return the result
    return result;
}

vec3 blinnPhong(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in  Material lightMaterial) {

    // Basic Properties
    const vec3 lightColor = lightMaterial.emmisive;
    const vec3 objectColor = objectMaterial.albedo;

    // Ambient
    const vec3 ambient = 0.15 * lightColor * objectColor;

    // Diffuse
    const vec3 lightDir = normalize(lightPos - worldPos);
    const float nDotL = max(dot(normal, lightDir), 0.0);
    const vec3 diffuse = lightColor * objectColor * nDotL;

    // Get the view & halfway vectors
    const vec3 viewDir = normalize(viewPos - worldPos);
    const vec3 halfwayDir = normalize(lightDir + viewDir); 

    // Specular (Blinn-Phong)
    const float specularStrength = pow(max(dot(normal, halfwayDir), 0.0), 16.0); 
    const float specularPower = abs(objectMaterial.roughness - 1.0); // TODO: Fix
    const vec3 specular = lightColor * specularPower * specularStrength; 

    // Final color
    const vec3 result = ambient + diffuse + specular;

    // Return the result
    return result;
}

// END INCLUDE: ../include/models/blinn_phong.glsl
// BEGIN INCLUDE: ../include/models/fresnel.glsl
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    float t = clamp(1.0 - cosTheta, 0.0, 1.0);
    float t2 = t * t;
    return F0 + (vec3(1.0) - F0) * (t2 * t2 * t);
}
// END INCLUDE: ../include/models/fresnel.glsl
// BEGIN INCLUDE: ../include/models/cook_torrance_bdrf.glsl
// GGX Microfacet sampling
vec3 sampleGGXWorld(vec3 normal, float roughness, float u1, float u2) {
    float a = roughness * roughness;
    float phi = TWO_PI * u1;

    float cosTheta = sqrt(max((1.0 - u2) / max(1.0 + (a * a - 1.0) * u2, EPSILON), 0.0));
    float sinTheta = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));

    vec3 localH = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
    return getBasis(normal) * localH;
}

// Microfacet Masking-Shadowing Function (Smith-GGX)
float smithGeometry(float NdotV, float NdotL, float roughness) {
    float a = max(roughness * roughness, EPSILON);
    float k = a * 0.5;
    float g1v = NdotV / max(NdotV * (1.0 - k) + k, EPSILON);
    float g1l = NdotL / max(NdotL * (1.0 - k) + k, EPSILON);
    return g1v * g1l;
}
// END INCLUDE: ../include/models/cook_torrance_bdrf.glsl

// Path tracing includes
// BEGIN INCLUDE: ../include/pathTracing/to_be_uniformed_valeus.glsl
#define MAX_BOUNCES 3
#define MIN_BOUNCE_RUSSIAN_ROULETTE 2
#define SKYBOX_COLOR_MULT 0.3
// END INCLUDE: ../include/pathTracing/to_be_uniformed_valeus.glsl
// BEGIN INCLUDE: ../include/pathTracing/structures.glsl
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

// END INCLUDE: ../include/pathTracing/structures.glsl
// BEGIN INCLUDE: ../include/pathTracing/sceneBuffers.glsl
// SSBO buffers
layout(std430, binding = 0) readonly buffer MeshEntryBuffer { GPUMeshEntry meshEntries[]; };
layout(std430, binding = 1) readonly buffer VertexBuffer { GPUVertex vertices[]; };
layout(std430, binding = 2) readonly buffer IndexBuffer { uint indices[]; };
layout(std430, binding = 3) readonly buffer InstanceBuffer { GPUInstance instances[]; };

// Total instance count
uniform int u_instanceCount;
// END INCLUDE: ../include/pathTracing/sceneBuffers.glsl

// Path tracing intersection functions
// BEGIN INCLUDE: ../include/pathTracing/intersections.glsl
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
    for (int instID = 0; instID < u_instanceCount; instID++) {

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

// END INCLUDE: ../include/pathTracing/intersections.glsl

// TODO: Make a path tracing uniforms file
uniform int u_frameNum;

// Final image writeout
layout(rgba32f, binding = 0) uniform image2D img_output;

// Cosine-weighted hemisphere sampling
vec3 sampleDiffuse(vec3 normal) {
    float u1 = rnd1(seed);
    float u2 = rnd1(seed);
    float r = sqrt(u2);
    float phi = TWO_PI * u1;

    vec3 localDir = vec3(r * cos(phi), r * sin(phi), sqrt(max(0.0, 1.0 - u2)));

    return getBasis(normal) * localDir;
}

// Modify values that need to be changed for the bounce
void processBounceStep(
    inout Ray ray,
    const in HitInfo hit,
    const in Material mat,
    inout vec3 colorMult
) {
    // Save the view and normal vectors
    vec3 V = -ray.direction;
    vec3 N = hit.normal;

    // Lambertion diffuse
    if (mat.metallic <= 0.5) {

        // Sample the diffuse pdf
        vec3 rayDirection = sampleDiffuse(N);

        // Update the ray information
        ray.origin = hit.hitPos + N * EPSILON;
        ray.direction = rayDirection;
        ray.invDirection = 1.0 / rayDirection;

        // Modify the throughput
        colorMult *= mat.albedo;
    }

    // Specular diffuse / Metal
    else if (mat.metallic > 0.5) {

        // Perfect reflection (or close enough)
        if (mat.roughness < 0.01) {

            // Perfectly reflect the ray
            vec3 rayDirection = reflect(-V, N);

            // Update the ray information
            ray.origin = hit.hitPos + N * EPSILON;
            ray.direction = rayDirection;
            ray.invDirection = 1.0 / rayDirection; 

            // Modify the throughput
            colorMult *= fresnelSchlick(max(dot(N, rayDirection), 0.0), mat.albedo);
        }

        // Non-perfect reflections
        else {

            // Sample GGX world (D)
            float u1 = rnd1(seed);
            float u2 = rnd1(seed);
            vec3 H = sampleGGXWorld(N, mat.roughness, u1, u2);

            // Get NdotL to check for a ray not being visable
            vec3 rayDirection = reflect(-V, H);
            float NdotL = dot(N, rayDirection);
            if (NdotL <= 0.0) {
                colorMult = vec3(0.0);
                return; 
            }

            // Update ray information
            ray.origin = hit.hitPos + N * EPSILON;
            ray.direction = rayDirection;
            ray.invDirection = 1.0 / rayDirection; 

            // Values for the next BDRF calculations
            float NdotV = max(dot(N, V), EPSILON);
            float NdotH = max(dot(N, H), EPSILON);
            float HdotV = max(dot(H, V), EPSILON);

            // Get the G & F components
            float G = smithGeometry(NdotV, NdotL, mat.roughness);
            vec3 F = fresnelSchlick(HdotV, mat.albedo);

            // Update the color mult (BRDF * cos / PDF simplifies cleanly to this)
            colorMult *= (F * G * HdotV) / (NdotV * NdotH);
        }
    }
}


/* ----------------- COLORING FUNCTIONS ----------------- */

// Sky color
vec3 colorSky(Ray ray) {
    vec3 sunDir = normalize(vec3(0.4, 0.8, 0.3));
    float sunDot = max(dot(ray.direction, sunDir), 0.0);

    // Horizon/zenith gradient
    float t = max(ray.direction.y, 0.0);
    vec3 sky = mix(vec3(0.7, 0.85, 1.0), vec3(0.15, 0.35, 0.7), pow(t, 0.6));

    // Sun disc + halo
    sky += vec3(1.2, 1.0, 0.6) * pow(sunDot, 128.0) * 8.0;  // disc
    sky += vec3(0.9, 0.7, 0.4) * pow(sunDot, 4.0)   * 0.4;  // halo

    return sky * SKYBOX_COLOR_MULT;
}

// Main color function / "render" function
vec3 colorScene(const in Ray cameraRay) {
    // Set higher-scope mutable structs
    Ray ray = cameraRay;
    Material material;
    HitInfo hit;

    // Color & throughput
    vec3 color = vec3(0.0);
    vec3 colorMult = vec3(1.0);

    for (int bounce = 0; bounce < MAX_BOUNCES; bounce++) {
        hit = rayScene(ray);

        if (!hit.hit) {
            color += colorSky(ray) * colorMult;
            break;
        }

        // TODO: replace
        if (hit.objectIndex == 0) {
            material = Material(
                vec3(0.4, 0.2, 0.8),
                vec3(0.0),
                1.0,
                0.0
            );
        } else {
            material = Material(
                vec3(0.0),
                vec3(100.0),
                0.0,
                0.0
            );
        }

        if (length(material.emmisive) > EPSILON) {
            color += material.emmisive * colorMult;
            break;
        }

        processBounceStep(ray, hit, material, colorMult);

        // Russian roulette
        if (bounce > MIN_BOUNCE_RUSSIAN_ROULETTE) { // Don't do it too early
            // Find the largest color weight
            float p = max(colorMult.r, max(colorMult.g, colorMult.b));

            // If the random chance is bigger than the largest color weight
            if (rnd1(seed) > p)
                break;

            // Account for lost energy
            colorMult /= p;
        }
    }

    return color;
}

void main() {

    // Calculate pixel uv
    const ivec2  pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    const ivec2 img_size = imageSize(img_output);

    // TODO: Make the uniform a uint
    setSeed(pixel_coords, uint(u_frameNum));

    // Error check on image size
    if (pixel_coords.x >= img_size.x || pixel_coords.y >= img_size.y) return;

    // Get screen space coords
    const vec2 uv = (vec2(pixel_coords) + 0.5) / vec2(img_size) * 2.0 - 1.0;

    // Get the aspect ratio
    const float aspectRatio = float(img_size.x) / float(img_size.y);

    // Get the ray direction that's in view space and put it into world space
    const float tanHalfFOV = tan(radians(u_FOV * 0.5)); // Helper value
    const vec3 rayDirView = normalize(vec3(uv, -1.0) * vec3(aspectRatio * tanHalfFOV, tanHalfFOV, 1.0));
    const vec3 rayDirWorld = normalize((u_inverseView * vec4(rayDirView, 0.0)).xyz);

    // Construct the final ray
    const Ray ray = Ray(u_cameraPos, rayDirWorld, 1.0 / rayDirWorld);

    // // Find the hit in the scene
    // const HitInfo hit = rayScene(ray);

    // Color based on the hit
    vec3 col = colorScene(ray);
    // if (hit.hit) {
    //     // Phong-shade to get the color
    //     col = blinnPhong(ray, hit, Material(vec3(0.4, 0.2, 0.8), vec3(0.0), 0.5, 0.0), ray.origin, Material(vec3(0.0), vec3(1.0), 0.0, 0.0));
    // } else {
    //     // Sky Gradient
    //     const float t = 0.5 * (ray.direction.y + 1.0);
    //     col = (1.0 - t) * vec3(1.0) + t * vec3(0.5, 0.7, 1.0);
    // }

    // Write to the image with the final hit color
    // imageStore(img_output, pixel_coords, vec4(rnd1(seed), rnd1(seed), rnd1(seed), 1.0));
    imageStore(img_output, pixel_coords, vec4(col, 1.0));
}
