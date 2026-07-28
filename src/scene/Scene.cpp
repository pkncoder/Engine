#include "Scene.h"

#include "../services/Logger.h"
#include "../services/UUID.h"

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
    // Generate the UUID via service
    EntityID uuid = UUID();

    // Set the internal index for this entity THEN increase for next itteration
    uint32_t internalIndex = nextAvailableIndex++;

    // Map this UUID to the next availiable index
    entityToIndex[uuid] = internalIndex;
    indexToEntity.push_back(uuid);

    // Reset signature for the internal index
    signatures[internalIndex].reset();

    // Count living entity count, as next availiable index can't be trusted
    livingEntityCount++;

    // Return the uuid for use
    return uuid;
}
} // namespace Engine
