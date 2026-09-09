#pragma once

#include <cassert>
#include <entt/entity/fwd.hpp>

#include "entt/entt.hpp"

namespace Engine {

// --- The Scene (Registry) ---
class Scene {
  public:
    Scene();
    ~Scene();

    inline entt::registry &getRegistry() { return registry; }

  private:
    entt::registry registry;
};

} // namespace Engine
