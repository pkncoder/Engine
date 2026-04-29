#pragma once
#include "ECS.h"
#include <array>
#include <cassert>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace Engine {

// "Pool" of memory parent class for polymorphism
class IPool {
  public:
    virtual ~IPool() = default;
};

// Component pool of type <T> class
template <typename T> class ComponentPool : public IPool {
  public:
    // Constructor & deconstructor
    ComponentPool() { m_Data = new T[MAX_ENTITIES]; }
    ~ComponentPool() override { delete[] m_Data; }

    // Data
    T *m_Data;
};

// --- The Scene (Registry) ---
class Scene {
  public:
    Scene();
    ~Scene();

    // Registering a new component type & accociated pool
    template <typename T> void registerComponent() {

        // Get the id of this component type
        ComponentType type = GetComponentTypeID<T>();
        assert(m_ComponentPools.find(type) == m_ComponentPools.end() &&
               "Registering component type more than once."); // Error checking

        // Create a new pool & add it to the list/map
        m_ComponentPools[type] = std::make_shared<ComponentPool<T>>();
    }

    // Adding a component to an entity id
    template <typename T> void addComponent(EntityID entity, T component) {

        // Get the component type's id
        ComponentType type = GetComponentTypeID<T>();

        // Flip the bit at the component id on the signature of the entity
        m_Signatures[entity].set(type, true);

        // Set the component at that id
        auto pool =
            std::static_pointer_cast<ComponentPool<T>>(m_ComponentPools[type]);
        pool->m_Data[entity] = component;
    }

    // Returns a referance to a component type with the entity id
    template <typename T> T &getComponent(EntityID entity) {

        // Get the component type id
        ComponentType type = GetComponentTypeID<T>();
        assert(m_Signatures[entity].test(type) &&
               "Entity does not have this component."); // Error checking

        // Get the component at that id and return it
        auto pool =
            std::static_pointer_cast<ComponentPool<T>>(m_ComponentPools[type]);
        return pool->m_Data[entity];
    }

    // Get a list of all component ids that match a list of component types
    // TODO: Fix name
    template <typename... ComponentTypes>
    std::vector<EntityID> getView() const {
        Signature requiredSignature; // Wanted signature

        // Set the bit for each component type's id (C++ fold)
        (requiredSignature.set(GetComponentTypeID<ComponentTypes>()), ...);

        // List of entity idsthat will be returned
        std::vector<EntityID> matchingEntities;

        // Loop through every existing entity
        for (EntityID i = 1; i < m_LivingEntityCount; ++i) {
            // Compare signatures with a bitwise and, if they are the same, then
            // add it to the list
            if ((m_Signatures[i] & requiredSignature) == requiredSignature) {
                matchingEntities.push_back(i);
            }
        }

        // Return the list fo entity ids
        return matchingEntities;
    }

    // Propagating a new entity (returning the new id)
    EntityID createEntity();

    // Getter for the total living entity count
    uint32_t getLivingEntityCount() const { return m_LivingEntityCount; }

  private:
    // Count of total entities, used to count and create custom id'ed entites
    EntityID m_LivingEntityCount = 1; // Start at 1, so 0 remains NULL_ENTITY

    // Component and signature pool
    std::array<Signature, MAX_ENTITIES> m_Signatures;
    std::unordered_map<ComponentType, std::shared_ptr<IPool>> m_ComponentPools;
};

} // namespace Engine
