#pragma once

#include "../../resources/CPUStructs.h"

#include <string>

namespace Engine {

// Mesh component struct
struct MeshComponent {
  public:
    MeshComponent() = default;
    inline MeshComponent(std::string _name, AssetHandle _handle)
        : name(_name), handle(_handle) {}

    // Name declaration
    std::string name = "";

    AssetHandle handle = INVALID_ASSET_HANDLE;
};

} // namespace Engine
