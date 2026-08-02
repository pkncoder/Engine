#pragma once

#include "../Constants.h"
#include "../resources/CPUStructs.h"
#include "ECS.h"

#include <array>
#include <cassert>
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
    inline ComponentPool() { data = new T[Constants::Entity::MAX_ENTITIES]; }
    inline ~ComponentPool() override { delete[] data; }

  public:
    T *data;
};

// --- The Scene (Registry) ---
class Scene {
  public:
    Scene();
    ~Scene();

    EntityID createEntity();
    void destroyEntity(const EntityID entity);

    // Registering a new component type & accociated pool
    template <typename T> inline void registerComponent() {

        // Get the id of this component type
        ComponentType type = getComponentTypeID<T>();
        assert(componentPools.find(type) == componentPools.end() &&
               "Registering component type more than once."); // Error checking

        // Create a new pool & add it to the list/map
        componentPools[type] = std::make_shared<ComponentPool<T>>();
    }

    // Adding a component to an entity id
    template <typename T>
    inline void addComponent(const EntityID entity, const T component) {

        // Resolve UUID -> internal index
        uint32_t internalIndex = entityToIndex[entity];

        // Get the component type's id
        ComponentType type = getComponentTypeID<T>();

        // Flip the bit at the component id on the signature of the entity
        signatures[internalIndex].set(type, true);

        // Set the component at that id
        auto pool =
            std::static_pointer_cast<ComponentPool<T>>(componentPools[type]);
        pool->data[internalIndex] = component;
    }

    // Returns a referance to a component type with the entity id
    template <typename T> inline T &getComponent(const EntityID entity) const {

        // Resolve UUID -> Internal Index
        uint32_t internalIndex = entityToIndex.find(entity)->second;

        // Get the component type id
        ComponentType type = getComponentTypeID<T>();
        assert(signatures[internalIndex].test(type) &&
               "Entity does not have this component."); // Error checking

        // Get the component at that id and return it
        auto pool = std::static_pointer_cast<ComponentPool<T>>(
            componentPools.find(type)->second);
        return pool->data[internalIndex];
    }

    // Get a list of all component ids that match a list of component types
    template <typename... ComponentTypes>
    inline std::vector<EntityID> getMatchingEntities() const {
        Signature requiredSignature; // Wanted signature

        // Set the bit for each component type's id (C++ fold)
        (requiredSignature.set(getComponentTypeID<ComponentTypes>()), ...);

        // List of entity ids that will be returned
        std::vector<EntityID> matchingEntities;

        // Loop using the internal memory index (uint32_t), starting at 0!
        for (uint32_t i = 0; i < nextAvailableIndex; ++i) {
            // Compare signatures with a bitwise and
            if ((signatures[i] & requiredSignature) == requiredSignature) {
                // Translate the internal index 'i' back into its UUID handle
                matchingEntities.push_back(indexToEntity[i]);
            }
        }

        // Return the list of entity ids
        return matchingEntities;
    }

    // TODO: Figure out how to deal with creating models / prefabs
    // WARN: Not created yet
    std::vector<EntityID> instantiateModel(AssetHandle modelHandle);

    inline uint32_t getLivingEntityCount() const { return livingEntityCount; }

  private:
    // Total entity count
    uint32_t livingEntityCount = 0;

    // Component and signature pool
    std::array<Signature, Constants::Entity::MAX_ENTITIES> signatures;
    std::unordered_map<ComponentType, std::shared_ptr<IPool>> componentPools;

    // Entity id (UUID) to internal index for data layout
    std::unordered_map<EntityID, uint32_t> entityToIndex;
    std::vector<EntityID>
        indexToEntity; // Inverse of entityToIndex for reverse lookups

    // Next free memory index in the vector
    uint32_t nextAvailableIndex = 0;
};

} // namespace Engine
