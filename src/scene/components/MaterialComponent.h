#pragma once

#include "../../resources/CPUStructs.h"

#include <glm/glm.hpp>

namespace Engine {

// Transformation struct, used for maths
struct MaterialComponent {
    glm::vec3 albedo{1.0f, 0.0f, 1.0f};
    glm::vec3 emmissive{0.0f, 0.0f, 0.0f};

    float roughness = 1.0f;
    float metallic = 0.0f;

    // Default Constructors
    MaterialComponent() = default;

    // Constructors
    MaterialComponent(CPUMaterialData materialData)
        : albedo(materialData.albedo), emmissive(materialData.emmissive),
          roughness(materialData.roughness), metallic(materialData.metallic) {}
    MaterialComponent(glm::vec3 _albedo, glm::vec3 _emmissive, float _roughness,
                      float _metallic)
        : albedo(_albedo), emmissive(_emmissive), roughness(_roughness),
          metallic(_metallic) {}
};

} // namespace Engine
