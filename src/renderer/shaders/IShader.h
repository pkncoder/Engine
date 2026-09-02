#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

namespace Engine {

enum ShaderType {
    Vertex = GL_VERTEX_SHADER,
    Geometry = GL_GEOMETRY_SHADER,
    Fragment = GL_FRAGMENT_SHADER,

    Compute = GL_COMPUTE_SHADER

};

class IShader {
  public:
    // Constructors
    IShader(const char *sourceFilename, const ShaderType type);
    ~IShader();

  public:
    std::string sourceFilename = "";
    const char *sourcePath = 0;
    std::string source = "";

    ShaderType type;
    GLuint glSourceShader = 0;

  private:
    // Recursively handles #include statements
    std::string getExpandedShaderCode(const std::string &shaderPath) const;

    // Writes the final stitched code to a file for easy debugging
    void dumpExpandedShaderCode() const;

    // Used to check errors on shader & program compilations
    void checkCompileErrors() const;
};

} // namespace Engine
