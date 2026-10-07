#pragma once

#include "IGPUResource.h"
#include <cstdint>
#include <glad/glad.h>

#include <string>
#include <vector>

namespace Engine {

struct RenderTargetDescriptor {
    float width = 1;
    float height = 1;

    bool createDepth = false;
};

struct GPURenderTarget : public IGPUResource {
  public:
    GPURenderTarget() = default;

  public:
    std::string name = "";
    std::uint64_t handle = 0;

    GLuint fbo = 0; // The actual framebuffer

    std::vector<GLuint> textureIDs = {}; // Multiple color attachments (MRTs)
    std::vector<GLenum> formats = {};    // Formats for each color attachment

    GLuint depthTextureID = 0; // Optional depth attachment
};

} // namespace Engine
