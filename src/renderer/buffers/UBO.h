#pragma once

#include <OpenGL/gltypes.h>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace Engine {

struct GPUUniformBuffer {
    GLuint frontBuffer;
    GLuint backBuffer;

    GLuint bindingPoint = 0;
    size_t size = 0;

    // We only need one CPU cache, because we push it before modifying it again
    std::vector<uint8_t> cpuCache;

    std::unordered_map<std::string, size_t> uniformOffsets;
};

} // namespace Engine
