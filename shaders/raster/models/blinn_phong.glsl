#include "../../include/modelBases/blinn_phong_base.glsl"

vec3 blinnPhong(const in vec3 viewPos, const in vec3 worldPos, const in vec3 normal, const in Material objectMaterial, const in vec3 lightPos, const in  Material lightMaterial) {
    // Pass in the math
    return blinnPhongMath(viewPos, worldPos, normal, objectMaterial, lightPos, lightMaterial);
}
