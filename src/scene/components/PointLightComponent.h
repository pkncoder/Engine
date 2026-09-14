#pragma once

#include <glm/ext/vector_float3.hpp>
namespace Engine {

struct PointLightComponent {
    glm::vec3 emissive = {1.0, 1.0, 1.0};
    float intensity = 1.0;
};

}
