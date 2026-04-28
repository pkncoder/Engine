#pragma once

#include "../resources/AssetTypes.h"
#include "../scene/components/MeshComponent.h"

#include <glm/glm.hpp>

namespace Engine {

class BufferManager {
  public:
    static MeshComponent uploadMesh(const MeshData &meshData);
};

} // namespace Engine
