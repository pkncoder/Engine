#pragma once
#include <glad/glad.h>
#include <string>

namespace Engine {
class TextureLoader {
  public:
    // Loads a texture from file and returns the OpenGL Texture ID
    static GLuint loadTexture(const std::string &filepath);
};
} // namespace Engine
