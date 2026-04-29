#include "Scene.h"

namespace Engine {

// Constructor & deconstructor
Scene::Scene() {
    // Initialize signatures to 0 (no components)
    for (auto &sig : m_Signatures) {
        sig.reset();
    }
}
Scene::~Scene() {}

// Propogating a new entity id (returning a new id)
// TODO: Return entity not entity id?
EntityID Scene::createEntity() {
    assert(m_LivingEntityCount < MAX_ENTITIES &&
           "Too many entities in existence."); // Error check

    // Grab the next available ID
    EntityID id = m_LivingEntityCount++;

    // Ensure its signature is clean
    m_Signatures[id].reset();

    // Return the id
    // TODO: Return entity not entity id?
    return id;
}

} // namespace Engine
