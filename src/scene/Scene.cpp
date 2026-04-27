#include "Scene.h"

namespace Engine {

Scene::Scene() {
    // Initialize signatures to 0 (no components)
    for (auto &sig : m_Signatures) {
        sig.reset();
    }
}

Scene::~Scene() {
    // Shared pointers in the unordered_map will automatically delete
    // the ComponentPools, safely firing the ~IPool destructor.
}

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
