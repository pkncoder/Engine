#pragma once
#include "../resources/AssetTypes.h"
#include <vector>

namespace Engine {

class Scene {
  public:
    // Constructor & Deconstructor
    Scene();
    ~Scene();

    // Debug mesh values
    // TODO: temp
    void addDebugMesh(const MeshData &mesh);
    void printDebugInfo() const;

  private:
    // Temporary storage until ECS is implemented
    // TODO: temp
    std::vector<MeshData> debugMeshes;
};

} // namespace Engine
