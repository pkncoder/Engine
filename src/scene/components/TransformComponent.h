#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Engine {

// Transformation struct, used for maths
struct Transform {
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f}; // quaternion
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
};

} // namespace Engine
