#pragma once

#include <bitset>
#include <cstdint>

namespace Engine {

// Entity ID typedef and settings
using EntityID = uint32_t;
const EntityID NULL_ENTITY = 0;
const EntityID MAX_ENTITIES = 10000;

// Components max num
const uint8_t MAX_COMPONENTS = 32;

// Signature typedef (bitset)
using Signature = std::bitset<MAX_COMPONENTS>;
using ComponentType = uint8_t;

// Getting a unique component id
inline ComponentType getUniqueComponentTypeID() {
    static ComponentType lastID = 0; // Static, shared value that increases
    return lastID++; // Increase the static lastID, and return the new value
}

// Get the component type id for a specific component type
// TODO: is this right? how does it return the old one if it already exists
template <typename T> inline ComponentType getComponentTypeID() {
    // Wrapper function for getting the component type id
    static const ComponentType typeID = getUniqueComponentTypeID();
    return typeID;
}

} // namespace Engine
