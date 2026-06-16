#include "../../include/modelBases/cook_torrance_bdrf_base.glsl"

vec3 cook_torrance_bdrf(inout Ray ray, in HitInfo hit, in GPUMaterial objectMaterial, in float u1, in float u2, out vec3 rayDirection) {

    vec3 originalRayDirection = ray.direction;

    vec3 microfacetNormal = sampleGGXWorld(hit.normal, objectMaterial.roughness, u1, u2);

    // Get NdotL to check for a ray not being visable
    rayDirection = reflect(ray.direction, microfacetNormal);
    float NdotL = dot(hit.normal, rayDirection);
    if (NdotL <= 0.0) {
        rayDirection = originalRayDirection;
        return vec3(0.0);
    }

    return cookTorranceBdrfBase(hit.normal, originalRayDirection, microfacetNormal, objectMaterial, u1, u2);
}
