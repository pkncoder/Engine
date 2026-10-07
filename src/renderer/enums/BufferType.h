#pragma once

#include <glad/glad.h>

namespace Engine {

enum class BufferType {
    UniformBuffer = GL_UNIFORM_BUFFER,
    StorageBuffer = GL_SHADER_STORAGE_BUFFER,
    VertexBuffer = GL_ARRAY_BUFFER,
    IndexBuffer = GL_ELEMENT_ARRAY_BUFFER,
    TextureBuffer = GL_TEXTURE_BUFFER
};

}
