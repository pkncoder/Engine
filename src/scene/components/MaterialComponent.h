#pragma once

#include "../../resources/CPUStructs.h"

namespace Engine {

// Transformation struct, used for maths
struct MaterialComponent {
  public:
    // Default Constructors
    MaterialComponent() = default;

    // Constructors
    MaterialComponent(const AssetHandle materialHandle)
        : handle(materialHandle) {}

  public:
    AssetHandle handle = INVALID_ASSET_HANDLE;
};

} // namespace Engine
