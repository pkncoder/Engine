#pragma once

#include "AssetTypes.h"

#include <optional>
#include <string>

namespace Engine {

class AssetManager {
  public:
    // Initialize the manager
    static void init();

    // Load and return a std::optional in case of failed loading
    static std::optional<MeshData> loadMesh(const std::string &filepath);
};

} // namespace Engine
