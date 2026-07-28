#pragma once

#include "../Constants.h"

#include <bitset>
#include <cstdint>

namespace Engine {

// TODO: Put into an extra ECS namespace?

// Entity ID typedef and settings
using EntityID = uint64_t;
const EntityID NULL_ENTITY = 0;

// Signature typedef (bitset)
using Signature = std::bitset<Constants::Entity::MAX_COMPONENTS>;
using ComponentType = uint8_t;

// Getting a unique component id
inline ComponentType getUniqueComponentTypeID() {
    static ComponentType lastID = 0; // Static, shared value that increases
    return lastID++; // Increase the static lastID, and return the new value
}

// Get the component type id for a specific component type
template <typename T> inline ComponentType getComponentTypeID() {
    // Wrapper function for getting the component type id
    static const ComponentType typeID = getUniqueComponentTypeID();
    return typeID;
}

} // namespace Engine
