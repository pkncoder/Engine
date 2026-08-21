#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

namespace Engine {

class Shader {
  public:
    // Constructors
    inline Shader() : ID(0) {}

    // TODO: Custom setup for individual things
    Shader(const char *vertexPath, const char *geometryPath,
           const char *fragmentPath);
    Shader(const char *vertexPath, const char *fragmentPath);
    Shader(const char *computePath);

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
    // Recursively handles #include statements
    std::string getExpandedShaderCode(const std::string &shaderPath) const;

    // Writes the final stitched code to a file for easy debugging
    void dumpExpandedShaderCode(const std::string &sourceFileName,
                                const std::string &source,
                                const std::string &type)
        const; // TODO: Swap to using custom enum on type

    // Used to check errors on shader & program compilations
    void checkCompileErrors(const uint32_t shader,
                            const std::string type)
        const; // TODO: Swap to using custom enum on type
};

} // namespace Engine
