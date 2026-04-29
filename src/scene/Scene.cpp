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

EntityID Scene::createEntity() {
    assert(m_LivingEntityCount < MAX_ENTITIES &&
           "Too many entities in existence.");

    // Grab the next available ID
    EntityID id = m_LivingEntityCount++;

    // Ensure its signature is clean
    m_Signatures[id].reset();

    return id;
}

} // namespace Engine
