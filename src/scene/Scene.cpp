#include "Scene.h"

namespace Engine {

// Constructor & deconstructor
Scene::Scene() {
    // Initialize signatures to 0 (no components)
    for (auto &sig : signatures) {
        sig.reset();
    }
}
Scene::~Scene() {}

// Propogating a new entity id (returning a new id)
EntityID Scene::createEntity() {
    assert(livingEntityCount < MAX_ENTITIES &&
           "Too many entities in existence."); // Error check

    // Grab the next available ID
    EntityID id = livingEntityCount++;

    // Ensure its signature is clean
    signatures[id].reset();

    // Return the id
    return id;
}

} // namespace Engine
