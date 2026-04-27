#pragma once
#include "../resources/AssetTypes.h"
#include <vector>

namespace Engine {

class Scene {
  public:
    Scene();
    ~Scene();

    // Temporary method for Step 1
    void addDebugMesh(const MeshData &mesh);
    void printDebugInfo() const;

  private:
    // Temporary storage until ECS is implemented
    std::vector<MeshData> debugMeshes;
};

} // namespace Engine
