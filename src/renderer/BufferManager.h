#pragma once

#include "../resources/AssetTypes.h"
#include "../scene/components/MeshComponent.h"

#include <glm/glm.hpp>

namespace Engine {

class BufferManager {
  public:
    // Getting the mesh component from mesh data & uploading the information
    static MeshComponent uploadMesh(const MeshData &meshData);
};

} // namespace Engine
