#pragma once

#include <bitset>
#include <cstdint>

namespace Engine {

// Identifiers and Limits
using EntityID = uint32_t;
const EntityID NULL_ENTITY = 0;
const EntityID MAX_ENTITIES = 10000;
const uint8_t MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;
using ComponentType = uint8_t;

// --- The Type ID Generator ---
// Every time we call GetComponentTypeID<NewType>(), it assigns a new integer
// (0, 1, 2...)
inline ComponentType GetUniqueComponentTypeID() {
    static ComponentType lastID = 0;
    return lastID++;
}

template <typename T> inline ComponentType GetComponentTypeID() {
    static const ComponentType typeID = GetUniqueComponentTypeID();
    return typeID;
}

} // namespace Engine
