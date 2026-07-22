#include "Scene.h"

#include "../services/Logger.h"
#include "ECS.h"

namespace Engine {

// Constructor & deconstructor
Scene::Scene() {
    // Initialize signatures to 0 (no components)
    for (auto &sig : signatures) {
        sig.reset();
    }

    Logger::info("SCENE", "Scene initialized");
}
Scene::~Scene() {}

// Propogating a new entity id (returning a new id)
EntityID Scene::createEntity() {

    if (livingEntityCount > Constants::Entity::MAX_ENTITIES) {
        Logger::error("SCENE", "Too many entities in existence.");
        return NULL_ENTITY;
    }

    // Grab the next available ID
    EntityID id = livingEntityCount++;

    // Ensure its signature is clean
    signatures[id].reset();

    // Return the id
    return id;
}

} // namespace Engine
