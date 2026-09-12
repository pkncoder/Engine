#pragma once

#include <glm/ext/vector_float3.hpp>
namespace Engine {

struct PointLightComponent {
    glm::vec3 emissive = {1.0, 1.0, 1.0};
    float strength = 1.0;
};

}
