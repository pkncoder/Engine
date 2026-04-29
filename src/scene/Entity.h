#pragma once
#include "ECS.h"
#include "Scene.h"

namespace Engine {

class Entity {
  public:
    // Constructors
    Entity() = default;
    Entity(EntityID id, Scene *scene) : m_ID(id), m_Scene(scene) {}

    // Getter for the id
    EntityID getID() const { return m_ID; }

    // Checking if the id is valid (not NULL_ENTITY id & scene is added)
    bool isValid() const { return m_ID != NULL_ENTITY && m_Scene != nullptr; }

    // Wrapper methods that talk to the Scene
    template <typename T> void addComponent(T component) {
        m_Scene->addComponent<T>(m_ID, component);
    }
    template <typename T> T &getComponent() {
        return m_Scene->getComponent<T>(m_ID);
    }

  private:
    // ID information and scene information
    EntityID m_ID = NULL_ENTITY;
    Scene *m_Scene = nullptr;
};

} // namespace Engine
