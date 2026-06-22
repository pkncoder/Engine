#include "../../include/modelBases/cookTorranceBDRFBase.glsl"

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
