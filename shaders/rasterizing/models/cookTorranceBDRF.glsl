#include "../../include/modelBases/cookTorranceBDRFBase.glsl"

vec3 cookTorranceBDRF(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in Material lightMaterial) {
    return cookTorranceBDRFBase(viewPos, worldPos, normal, objectMaterial, lightPos, lightMaterial);
}

