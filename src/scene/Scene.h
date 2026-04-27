#pragma once
#include "ECS.h"
#include <array>
#include <cassert>
#include <memory>
#include <unordered_map>

namespace Engine {

// --- Type-Safe Memory Pools ---
class IPool {
  public:
    virtual ~IPool() = default;
};

template <typename T> class ComponentPool : public IPool {
  public:
    ComponentPool() { m_Data = new T[MAX_ENTITIES]; }
    ~ComponentPool() override { delete[] m_Data; }
    T *m_Data;
};

// --- The Scene (Registry) ---
class Scene {
  public:
    Scene();
    ~Scene();

    // Entity Management
    EntityID createEntity();

    // Component Management
    template <typename T> void registerComponent() {
        ComponentType type = GetComponentTypeID<T>();
        assert(m_ComponentPools.find(type) == m_ComponentPools.end() &&
               "Registering component type more than once.");

        m_ComponentPools[type] = std::make_shared<ComponentPool<T>>();
    }

    template <typename T> void addComponent(EntityID entity, T component) {
        ComponentType type = GetComponentTypeID<T>();

        // 1. Update the entity's signature to show it owns this component
        m_Signatures[entity].set(type, true);

        // 2. Insert the data into the flat array
        auto pool =
            std::static_pointer_cast<ComponentPool<T>>(m_ComponentPools[type]);
        pool->m_Data[entity] = component;
    }

    template <typename T> T &getComponent(EntityID entity) {
        ComponentType type = GetComponentTypeID<T>();

        assert(m_Signatures[entity].test(type) &&
               "Entity does not have this component.");

        auto pool =
            std::static_pointer_cast<ComponentPool<T>>(m_ComponentPools[type]);
        return pool->m_Data[entity];
    }

  private:
    std::array<Signature, MAX_ENTITIES> m_Signatures;
    EntityID m_LivingEntityCount = 1; // Start at 1, so 0 remains NULL_ENTITY

    std::unordered_map<ComponentType, std::shared_ptr<IPool>> m_ComponentPools;
};

} // namespace Engine
