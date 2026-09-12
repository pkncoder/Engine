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

    inline void setActiveCameraID(entt::entity cameraID) {
        cameraID = activeCameraID;
    }

    inline entt::registry &getRegistry() { return registry; }

  public:
    entt::entity activeCameraID;

  private:
    entt::registry registry;
};

} // namespace Engine
