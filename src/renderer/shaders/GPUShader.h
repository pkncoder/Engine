#pragma once

#include "../enums/ShaderType.h"
#include "../gpuResources/IGPUResource.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

namespace Engine {

class GPUShader : public IGPUResource {
  public:
    // Constructors
    GPUShader(const char *sourceFilename, const ShaderType type);
    ~GPUShader();

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
