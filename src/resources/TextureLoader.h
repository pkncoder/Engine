#pragma once

#include <glad/glad.h>

#include <string>

namespace Engine {
class TextureLoader {
  public:
    // Loads a texture from file and return the OpenGL Texture ID
    static GLuint loadTexture(const std::string &filepath, bool &isBumpTexture);
};
} // namespace Engine
