#pragma once

#include <glm/ext/vector_float3.hpp>
namespace Engine {

struct PointLightComponent {
  public:
    inline PointLightComponent() = default;

    inline PointLightComponent(const glm::vec3 _emissive,
                               const float _intensity)
        : emissive(_emissive), intensity(_intensity) {}

  public:
    glm::vec3 emissive = {1.0, 1.0, 1.0};
    float intensity = 1.0;
};

} // namespace Engine
