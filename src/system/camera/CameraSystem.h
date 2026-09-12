#pragma once

#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>

namespace Engine {

class CameraSystem {
  public:
    // Run this every frame
    void update(entt::registry &registry);
};

} // namespace Engine
