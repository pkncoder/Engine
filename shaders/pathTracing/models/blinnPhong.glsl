#include "../../include/modelBases/blinnPhongBase.glsl"

vec3 blinnPhong(const in Ray ray, const in HitInfo hit, const in Material objectMaterial, const in vec3 lightPos, const in Material lightMaterial) {
    // Pass through the math
    return blinnPhongBase(ray.origin, hit.hitPos, hit.normal, objectMaterial, lightPos, lightMaterial);
}
