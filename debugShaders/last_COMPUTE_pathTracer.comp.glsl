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

    uint objectIndex;
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

// Other utils
// BEGIN INCLUDE: ../../include/utils/random.glsl
uint seed; // Global seed

// Backup seed for when the blue noise texture isn't loaded
void setSeed(vec2 fragCoord, uint frameNum)
{
    seed = uint(fragCoord.x) * 1973u + uint(fragCoord.y) * 9277u + frameNum * 26699u | 1u;
}

// Hash function for random uint
uint wangHash(inout uint seed) {
    seed = (seed ^ 61u) ^ (seed >> 16u);
    seed *= 9u;
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}

// Random float one from [0-1]
float rnd1(inout uint seed){
    return float(wangHash(seed)) / float(-1u);
}

// Random normalized unit vector
vec3 rndUnit(inout uint seed) {
    float z = rnd1(seed) * 2.0f - 1.0f;
    float a = rnd1(seed) * 6.28318530718;
    float r = sqrt(1.0f - z * z);
    return vec3(r * cos(a), r * sin(a), z);
}
// END INCLUDE: ../../include/utils/random.glsl
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

// Coloring models
// #include "../include/models/blinn_phong.glsl"
// BEGIN INCLUDE: ../models/cookTorranceBDRF.glsl
// BEGIN INCLUDE: ../../include/modelBases/cookTorranceBDRFBase.glsl
// ============================================================================
// SHARED PBR MATHEMATICAL PRIMITIVES
// ============================================================================

// GGX Microfacet distribution function (D)
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / max(denom, EPSILON);
}

// Microfacet Masking-Shadowing Function (Smith-GGX) (G)
// Remaps 'k' differently for analytical direct lights vs stochastic sampled rays
float GeometrySchlickGGX(float NdotV, float roughness, bool isDirectLight) {
    float r = roughness;
    float k;
    if (isDirectLight) {
        k = ((r + 1.0) * (r + 1.0)) / 8.0; // Rasterizer remapping
    } else {
        k = (r * r) / 2.0;                 // Path Tracer / IBL remapping
    }

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / max(denom, EPSILON);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness, bool isDirectLight) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness, isDirectLight);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness, isDirectLight);

    return ggx2 * ggx1;
}

// Fresnel (F)
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (vec3(1.0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Stochastic GGX Microfacet Sampling for Path Tracing
vec3 sampleGGXWorld(vec3 normal, float roughness, float u1, float u2) {
    float a = roughness * roughness;
    float phi = TWO_PI * u1;

    float cosTheta = sqrt(max((1.0 - u2) / max(1.0 + (a * a - 1.0) * u2, EPSILON), 0.0));
    float sinTheta = sqrt(max(0.0, 1.0 - cosTheta * cosTheta));

    vec3 localH = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
    return getBasis(normal) * localH;
}

// ============================================================================
// OVERLOAD 1: RASTERIZER / ANALYTICAL DIRECT LIGHTING MODEL
// ============================================================================
vec3 cookTorranceBDRFBase(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in Material lightMaterial) {
    vec3 N = normalize(normal);
    vec3 V = normalize(viewPos - worldPos);
    vec3 L = normalize(lightPos - worldPos);
    vec3 H = normalize(V + L);

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, objectMaterial.albedo.rgb, objectMaterial.metallic);

    float D = DistributionGGX(N, H, objectMaterial.roughness);
    float G = GeometrySmith(N, V, L, objectMaterial.roughness, true); // True = Direct analytical lighting
    vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = D * G * F;
    float denominator = 4.0 * NdotV * NdotL;
    vec3 specular = numerator / max(denominator, EPSILON);

    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - objectMaterial.metallic);

    vec3 lightColor = lightMaterial.emissive.rgb; 
    vec3 diffuse = objectMaterial.albedo.rgb / PI;
    vec3 ambient = 0.03 * objectMaterial.albedo.rgb;

    return ambient + (kD * diffuse + specular) * lightColor * NdotL;
}

// ============================================================================
// OVERLOAD 2: PATH TRACER STOCHASTIC RAY GENERATION (BRDF Importance Sampling)
// ============================================================================
// Takes random uniforms, generates a bounce direction, returns attenuation weight
vec3 cookTorranceBDRFBase(const in vec3 normal, const in vec3 viewDirection, const in Material objectMaterial, float u1, float u2, out vec3 outBounceDirection) {
    vec3 N = normalize(normal);
    vec3 V = normalize(-viewDirection); // Ensure V points outward away from the surface

    // Sample the microfacet normal (H) using GGX distribution
    vec3 H = sampleGGXWorld(N, objectMaterial.roughness, u1, u2);
    
    // Reflect incoming ray about the microfacet to get the outgoing light ray
    outBounceDirection = reflect(viewDirection, H); 
    vec3 L = normalize(outBounceDirection);

    float NdotL = dot(N, L);
    if (NdotL <= 0.0) {
        return vec3(0.0); // Absorbed/Reflected into surface
    }

    float NdotV = max(dot(N, V), EPSILON);
    float NdotH = max(dot(N, H), EPSILON);
    float HdotV = max(dot(H, V), EPSILON);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, objectMaterial.albedo.rgb, objectMaterial.metallic);

    float G = GeometrySmith(N, V, L, objectMaterial.roughness, false); // False = Path traced indirect ray
    vec3 F  = fresnelSchlick(HdotV, F0);

    // Simplified weight multiplier: (BRDF * cos(theta)) / PDF 
    // Strips away 'D' and geometry division factors cleanly
    vec3 specularWeight = (F * G * HdotV) / (NdotV * NdotH);
    
    return specularWeight;
}

// // ============================================================================
// // OVERLOAD 3: PATH TRACER MIS EVALUATION (Direct Light Sampling / Next Event Estimation)
// // ============================================================================
// // Evaluates the precise raw BRDF value and PDF for a given explicit target direction
// vec3 cookTorranceBDRFBase(const in vec3 normal, const in vec3 viewDirection, const in vec3 explicitLightDirection, const in Material objectMaterial, out float outPdf) {
//     vec3 N = normalize(normal);
//     vec3 V = normalize(-viewDirection); // Ensure V points outward
//     vec3 L = normalize(explicitLightDirection);
//     vec3 H = normalize(V + L);
//
//     float NdotL = max(dot(N, L), 0.0);
//     float NdotV = max(dot(N, V), 0.0);
//
//     if (NdotL <= 0.0 || NdotV <= 0.0) {
//         outPdf = 0.0;
//         return vec3(0.0);
//     }
//
//     float NdotH = max(dot(N, H), EPSILON);
//     float HdotV = max(dot(H, V), EPSILON);
//
//     vec3 F0 = vec3(0.04);
//     F0 = mix(F0, objectMaterial.albedo, objectMaterial.metallic);
//
//     float D = DistributionGGX(N, H, objectMaterial.roughness);
//     float G = GeometrySmith(N, V, L, objectMaterial.roughness, false);
//     vec3 F  = fresnelSchlick(HdotV, F0);
//
//     // Calculate structural sampling probability density function for MIS weight balancing
//     outPdf = (D * NdotH) / (4.0 * HdotV);
//
//     // Evaluate standard raw PBR components
//     vec3 specular = (D * G * F) / max(4.0 * NdotV * NdotL, EPSILON);
//     vec3 kS = F;
//     vec3 kD = (vec3(1.0) - kS) * (1.0 - objectMaterial.metallic);
//     vec3 diffuse = objectMaterial.albedo / PI;
//
//     return (kD * diffuse) + specular;
// }
// END INCLUDE: ../../include/modelBases/cookTorranceBDRFBase.glsl

vec3 cookTorranceBDRF(inout Ray ray, in HitInfo hit, in Material objectMaterial, in float u1, in float u2, out vec3 rayDirection) {

    vec3 originalRayDirection = ray.direction;

    vec3 microfacetNormal = sampleGGXWorld(hit.normal, objectMaterial.roughness, u1, u2);

    // Get NdotL to check for a ray not being visable
    rayDirection = reflect(ray.direction, microfacetNormal);
    float NdotL = dot(hit.normal, rayDirection);
    if (NdotL <= 0.0) {
        rayDirection = originalRayDirection;
        return vec3(0.0);
    }

    return cookTorranceBDRFBase(hit.normal, originalRayDirection, objectMaterial, u1, u2, rayDirection);
}
// END INCLUDE: ../models/cookTorranceBDRF.glsl

// Path tracing includes
// BEGIN INCLUDE: ../to_be_uniformed_valeus.glsl
// TODO: uniform

#define MAX_BOUNCES 3
#define MIN_BOUNCE_RUSSIAN_ROULETTE 2
#define SKYBOX_COLOR_MULT 0.3
// END INCLUDE: ../to_be_uniformed_valeus.glsl
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
    for (uint instID = 0; instID < uInstanceCount; instID++) {

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
uniform uint uFrameNum;

// Final image writeout
layout(rgba32f, binding = 0) uniform image2D MainColorOutput;
layout(rgba32f, binding = 1) uniform image2D Normal;
layout(rgba32f, binding = 2) uniform image2D Albedo;
layout(rgba32f, binding = 3) uniform image2D Emissive;
layout(rgba32f, binding = 4) uniform image2D IMR;
layout(rgba32f, binding = 5) uniform image2D Depth;
layout(rgba32f, binding = 6) uniform image2D Hit;
// END INCLUDE: ../uniforms.glsl

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
    const in Material material,
    inout vec3 colorMult
) {

    // Lambertion diffuse
    if (material.metallic <= 0.5) {

        // Sample the diffuse pdf
        vec3 rayDirection = sampleDiffuse(hit.normal);

        // Update the ray information
        ray.origin = hit.hitPos + hit.normal * EPSILON;
        ray.direction = rayDirection;
        ray.invDirection = 1.0 / rayDirection;

        // Modify the throughput
        colorMult *= material.albedo.xyz;
    }

    // Specular diffuse / Metal
    else if (material.metallic > 0.5) {

        // Perfect reflection (or close enough)
        if (material.roughness < 0.01) {

            // Perfectly reflect the ray
            vec3 rayDirection = reflect(ray.direction, hit.normal);

            // Update the ray information
            ray.origin = hit.hitPos + hit.normal * EPSILON;
            ray.direction = rayDirection;
            ray.invDirection = 1.0 / rayDirection; 

            // Modify the throughput
            colorMult *= fresnelSchlick(max(dot(hit.normal, rayDirection), 0.0), material.albedo.xyz);
        }

        // Non-perfect reflections
        else {

            // Sample GGX world (D)
            float u1 = rnd1(seed);
            float u2 = rnd1(seed);

            vec3 newRayDirection;

            vec3 bdrf = cookTorranceBDRF(ray, hit, material, u1, u2, newRayDirection);

            ray.origin = hit.hitPos + hit.normal * EPSILON;
            ray.direction = newRayDirection;
            ray.invDirection = 1.0 / newRayDirection; 

            colorMult *= bdrf;
        }
    }
}

HitInfo getGBufferHit(const in Ray ray, ivec2 pixelCoords) {
    HitInfo hit;

    vec4 hitBuffer = imageLoad(Hit, pixelCoords);

    hit.hit = bool(hitBuffer.a);
    hit.dist = imageLoad(Depth, pixelCoords).r;

    hit.hitPos = ray.origin + ray.direction * hit.dist;
    hit.normal = imageLoad(Normal, pixelCoords).rgb;

    hit.objectIndex = uint(hitBuffer.r);
    hit.materialIndex = uint(hitBuffer.g);

    return hit;
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
vec3 colorScene(const in Ray cameraRay, ivec2 pixelCoords) {
    // Set higher-scope mutable structs
    Ray ray = cameraRay;
    Material material;
    HitInfo hit;

    // Color & throughput
    vec3 color = vec3(0.0);
    vec3 colorMult = vec3(1.0);

    hit = getGBufferHit(ray, pixelCoords);

    for (int bounce = 1; bounce < MAX_BOUNCES; bounce++) {

        if (!hit.hit) {
            color += colorSky(ray) * colorMult;
            break;
        }

        material = materials[hit.materialIndex];

        if (length(material.emissive.xyz) > EPSILON) {
            color += material.emissive.xyz * colorMult;
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

        hit = rayScene(ray);
    }

    return color;
}

void main() {

    // Calculate pixel uv
    const ivec2  pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    const ivec2 imgSize = imageSize(MainColorOutput);

    setSeed(pixelCoords, uFrameNum);

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

    // Color based on the hit
    vec3 col = colorScene(ray, pixelCoords);

    col = linearToSRGB(ACESFilm(col));

    // Write to the image with the final hit color
    // imageStore(MainColorOutput, pixelCoords, vec4(rnd1(seed), rnd1(seed), rnd1(seed), 1.0));
    imageStore(MainColorOutput, pixelCoords, vec4(col, 1.0));
}
