#include "Scene.h"

#include "../services/Logger.h"
#include <entt/entity/fwd.hpp>

namespace Engine {

// Constructor & deconstructor
Scene::Scene() {
    registry = entt::registry();
    Logger::info("SCENE", "Scene initialized");
}
Scene::~Scene() {}

} // namespace Engine
