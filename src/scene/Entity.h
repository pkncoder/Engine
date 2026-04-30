#pragma once
#include "ECS.h"
#include "Scene.h"

namespace Engine {

class Entity {
  public:
    // Constructors
    Entity() = default;
    Entity(EntityID id, Scene *scene) : ID(id), attachedScene(scene) {}

    // Getter for the id
    EntityID getID() const { return ID; }

    // Checking if the id is valid (not NULL_ENTITY id & scene is added)
    bool isValid() const {
        return ID != NULL_ENTITY && attachedScene != nullptr;
    }

    // Wrapper methods that talk to the Scene
    template <typename T> void addComponent(T component) {
        attachedScene->addComponent<T>(ID, component);
    }
    template <typename T> T &getComponent() {
        return attachedScene->getComponent<T>(ID);
    }

  private:
    // ID information and scene information
    EntityID ID = NULL_ENTITY;
    Scene *attachedScene = nullptr;
};

} // namespace Engine
