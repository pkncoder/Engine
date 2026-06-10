#pragma once

#include <glm/glm.hpp>

namespace Engine {

// Transformation struct, used for maths
struct MaterialComponent {
    glm::vec3 albedo{1.0f, 0.0f, 1.0f};
    glm::vec3 emmission{0.0f, 0.0f, 0.0f};

    float roughness = 1.0f;
    float metallic = 0.0f;
    float ior = 0.0f;
};

} // namespace Engine
