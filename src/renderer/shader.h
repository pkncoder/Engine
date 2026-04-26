#pragma once

#include <glm/glm.hpp>
#include <string>

class Shader {
  public:
    uint32_t ID; // The OpenGL Program ID

    Shader() : ID(0) {}
    Shader(const char *vertexPath, const char *fragmentPath);

    void bind() const;
    void unbind() const;

    // TODO: Finish writing the rest of the set functions

    // Uniform utility functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

  private:
    void checkCompileErrors(uint32_t shader, std::string type);
};
