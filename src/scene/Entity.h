#pragma once
#include "ECS.h"
#include "Scene.h"

namespace Engine {

class Entity {
  public:
    Entity() = default;
    Entity(EntityID id, Scene *scene) : m_ID(id), m_Scene(scene) {}

    bool isValid() const { return m_ID != NULL_ENTITY && m_Scene != nullptr; }
    EntityID getID() const { return m_ID; }

    // Wrapper methods that talk to the Scene
    template <typename T> void addComponent(T component) {
        m_Scene->addComponent<T>(m_ID, component);
    }

    template <typename T> T &getComponent() {
        return m_Scene->getComponent<T>(m_ID);
    }

  private:
    EntityID m_ID = NULL_ENTITY;
    Scene *m_Scene = nullptr;
};

} // namespace Engine
