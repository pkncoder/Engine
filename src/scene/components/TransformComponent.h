#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Engine {

struct Transform {
    glm::vec3 Position{0.0f, 0.0f, 0.0f};
    glm::quat Rotation{1.0f, 0.0f, 0.0f, 0.0f}; // Identity quaternion
    glm::vec3 Scale{1.0f, 1.0f, 1.0f};
};

} // namespace Engine
