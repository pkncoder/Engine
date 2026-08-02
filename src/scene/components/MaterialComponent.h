#pragma once

#include "../../resources/CPUStructs.h"

namespace Engine {

// Transformation struct, used for maths
struct MaterialComponent {
  public:
    // Default Constructor
    MaterialComponent() = default;

    // Constructor
    inline MaterialComponent(const AssetHandle materialHandle)
        : handle(materialHandle) {}

  public:
    // Asset manager handle to the material
    AssetHandle handle = INVALID_ASSET_HANDLE;
};

} // namespace Engine
