#pragma once

#include "IShader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Engine {

class IProgram {
  public:
    // Constructors
    IProgram();

    void attachShader(IShader shader);
    void link();

    // Binding and unbinding the program
    void bind() const;
    void unbind() const;

    // Uniform utility functions
    void setBool(const std::string &name, const bool value) const;
    void setInt(const std::string &name, const int value) const;
    void setUInt(const std::string &name, const int value) const;
    void setFloat(const std::string &name, const float value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

  public:
    uint32_t ID; // The OpenGL Program ID

  private:
    // Used to check errors on shader & program compilations
    void checkLinkErrors() const;

  private:
    std::vector<IShader> attachedShaders;
};

} // namespace Engine
