#include "../../include/modelBases/cook_torrance_bdrf_base.glsl"

vec3 cook_torrance_bdrf(inout Ray ray, in Hit hitInfo, in Material objectMaterial, in float u1, in float u2) {

    return cook_torrance_bdrf_base(hit.normal, ray.direction, objectMaterial, u1, u2);
}
