#pragma once

#include "../../resources/CPUStructs.h"

#include <string>

namespace Engine {

// Mesh component struct
struct MeshComponent {
  public:
    // Default constructor
    MeshComponent() = default;

    // Data constructor
    inline MeshComponent(const std::string _name, const AssetHandle _handle)
        : name(_name), handle(_handle) {}

  public:
    // Name declaration
    std::string name = "";

    // Asset manager handle to the mesh - NOT a model
    AssetHandle handle = INVALID_ASSET_HANDLE;
};

} // namespace Engine
