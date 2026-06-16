#include "../../include/modelBases/cook_torrance_bdrf_base.glsl"

// TODO: Fix/learn how to use
vec3 cookTorranceBdrf(vec3 normal, vec3 viewPos, vec3 worldPos, GPUMaterial objectMaterial, float u1, float u2) {
    vec3 viewDir = normalize(viewPos - worldPos);

    return cookTorranceBdrfBase(normal, viewDir, objectMaterial, u1, u2);
}

