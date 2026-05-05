#pragma once

#include <glm/glm.hpp>

#include <string>

namespace Engine {

class Shader {
  public:
    uint32_t ID; // The OpenGL Program ID

    // Constructors
    Shader() : ID(0) {}
    Shader(const char *vertexPath, const char *fragmentPath);
    Shader(const char *computePath);

    // Binding and unbinding the program
    void bind() const;
    void unbind() const;

    // Uniform utility functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

  private:
    // Recursively handles #include statements
    std::string getExpandedShaderCode(const std::string &shaderPath);

    // Writes the final stitched code to a file for easy debugging
    void dumpExpandedShaderCode(const std::string &source,
                                const std::string &type);

    // Used to check errors on shader & program compilations
    void checkCompileErrors(uint32_t shader, std::string type) const;
};

} // namespace Engine
