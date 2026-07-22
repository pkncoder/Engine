#pragma once

#include "../../resources/CPUStructs.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Engine {

// Transformation struct, used for maths
struct MaterialComponent {
  public:
    // Default Constructors
    MaterialComponent() = default;

    // Constructors
    MaterialComponent(const CPUMaterialData materialData)
        : albedo(materialData.albedo), emmissive(materialData.emmissive),
          roughness(materialData.roughness), metallic(materialData.metallic) {}
    MaterialComponent(const glm::vec3 _albedo, const glm::vec3 _emmissive,
                      const float _roughness, const float _metallic)
        : albedo(_albedo), emmissive(_emmissive), roughness(_roughness),
          metallic(_metallic) {}

  public:
    // Color info
    glm::vec3 albedo{1.0f, 0.0f, 1.0f};
    glm::vec3 emmissive{0.0f, 0.0f, 0.0f};

    // Material attributes
    float roughness = 1.0f;
    float metallic = 0.0f;

    // Textures
    GLuint albedoTexture = 0;
    GLuint emissiveTexture = 0;
    GLuint metallicTexture = 0;
    GLuint roughnessTexture = 0;
    GLuint alphaTexture = 0;

    // Normal / bump texture info
    GLuint normalTexture = 0;
    bool isBumpMap = false;
};

} // namespace Engine
