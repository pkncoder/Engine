#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Engine {

// Transformation struct, used for maths
struct TransformComponent {
  public:
    // Default constructor
    TransformComponent() = default;

    // Data constructor
    inline TransformComponent(const glm::vec3 _position,
                              const glm::quat _rotation, const glm::vec3 _scale)
        : position(_position), rotation(_rotation), scale(_scale){};

  public:
    // Transformation attributes
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f}; // quaternion
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
};

} // namespace Engine
