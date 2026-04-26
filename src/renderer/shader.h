#pragma once

#include <glm/glm.hpp>
#include <string>

class Shader {
  public:
    uint32_t ID; // The OpenGL Program ID

    // TODO: What mean
    Shader() : ID(0) {}
    Shader(const char *vertexPath, const char *fragmentPath);

    void Bind() const;
    void Unbind() const;

    // TODO: Finish writing the rest of the set functions

    // Uniform utility functions
    void SetBool(const std::string &name, bool value) const;
    void SetInt(const std::string &name, int value) const;
    void SetFloat(const std::string &name, float value) const;
    void SetVec3(const std::string &name, const glm::vec3 &value) const;
    void SetMat4(const std::string &name, const glm::mat4 &mat) const;

  private:
    void checkCompileErrors(uint32_t shader, std::string type);
};
