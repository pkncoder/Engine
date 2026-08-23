#include "Shader.h"

#include "../services/Logger.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace Engine {

namespace fs = std::filesystem;

Shader::Shader(const char *vertexPath, const char *geometryPath,
               const char *fragmentPath) {

    // Get the shader code without #includes
    std::string vertexCode = getExpandedShaderCode(vertexPath);
    std::string geometryCode = getExpandedShaderCode(geometryPath);
    std::string fragmentCode = getExpandedShaderCode(fragmentPath);

    // Get the filenames of each code file
    std::string vertexFileName =
        std::string(vertexPath)
            .substr(std::string(vertexPath).find_last_of('/') + 1);

    std::string geometryFileName =
        std::string(geometryPath)
            .substr(std::string(geometryPath).find_last_of('/') + 1);

    std::string fragmentFileName =
        std::string(fragmentPath)
            .substr(std::string(fragmentPath).find_last_of('/') + 1);

    // Dump the expanded shader code for debug
    dumpExpandedShaderCode(vertexFileName, vertexCode, "VERTEX");
    dumpExpandedShaderCode(geometryFileName, geometryCode, "GEOMETRY");
    dumpExpandedShaderCode(fragmentFileName, fragmentCode, "FRAGMENT");

    // Get c-strings of the shader code
    const char *vShaderSource = vertexCode.c_str();
    const char *gShaderSource = geometryCode.c_str();
    const char *fShaderSource = fragmentCode.c_str();

    // Save variables for the final shader
    uint32_t vertex, geometry, fragment;

    // Create and compile the vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderSource, NULL);
    glCompileShader(vertex);

    // Check for compilation errors
    checkCompileErrors(vertex, "VERTEX");

    // Create and compile the fragment shader
    geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &gShaderSource, NULL);
    glCompileShader(geometry);

    // Check for compilation errors
    checkCompileErrors(geometry, "GEOMETRY");

    // Create and compile the fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderSource, NULL);
    glCompileShader(fragment);

    // Check for compilation errors
    checkCompileErrors(fragment, "FRAGMENT");

    // Create an OpenGL program & attatch the shaders
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, geometry);
    glAttachShader(ID, fragment);

    // Link the program to OpenGL
    glLinkProgram(ID);

    // Check for projgram creation errors
    checkCompileErrors(ID, "PROGRAM");

    // Delete the old shaders, as the program now has them
    glDeleteShader(vertex);
    glDeleteShader(geometry);
    glDeleteShader(fragment);
}

// On construction - compile source code & program
Shader::Shader(const char *vertexPath, const char *fragmentPath) {

    // Get the shader code without #includes
    std::string vertexCode = getExpandedShaderCode(vertexPath);
    std::string fragmentCode = getExpandedShaderCode(fragmentPath);

    // Get the filenames of each code file
    std::string vertexFileName =
        std::string(vertexPath)
            .substr(std::string(vertexPath).find_last_of('/') + 1);

    std::string fragmentFileName =
        std::string(fragmentPath)
            .substr(std::string(fragmentPath).find_last_of('/') + 1);

    // Dump the expanded shader code for debug
    dumpExpandedShaderCode(vertexFileName, vertexCode, "VERTEX");
    dumpExpandedShaderCode(fragmentFileName, fragmentCode, "FRAGMENT");

    // Get c-strings of the shader code
    const char *vShaderSource = vertexCode.c_str();
    const char *fShaderSource = fragmentCode.c_str();

    // Save variables for the final shader
    uint32_t vertex, fragment;

    // Create and compile the vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderSource, NULL);
    glCompileShader(vertex);

    // Check for compilation errors
    checkCompileErrors(vertex, "VERTEX");

    // Create and compile the fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderSource, NULL);
    glCompileShader(fragment);

    // Check for compilation errors
    checkCompileErrors(fragment, "FRAGMENT");

    // Create an OpenGL program & attatch the shaders
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    // Link the program to OpenGL
    glLinkProgram(ID);

    // Check for projgram creation errors
    checkCompileErrors(ID, "PROGRAM");

    // Delete the old shaders, as the program now has them
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// Constructor for Compute Shaders
Shader::Shader(const char *computePath) {
    // Get the final compute code without #includes
    std::string computeCode = getExpandedShaderCode(computePath);

    // Get the filename and dump the code
    std::string fileName =
        std::string(computePath)
            .substr(std::string(computePath).find_last_of('/') + 1);
    dumpExpandedShaderCode(fileName, computeCode, "COMPUTE");

    // Get the compute shader source
    const char *cShaderSource = computeCode.c_str();

    // Create and compute the compute shader
    uint32_t compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &cShaderSource, NULL);
    glCompileShader(compute);

    // Check for errors
    checkCompileErrors(compute, "COMPUTE");

    // Create Program and link it
    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);

    // Check for errors
    checkCompileErrors(ID, "PROGRAM");

    // Cleanup
    glDeleteShader(compute);
}

// Bind and unbind
void Shader::bind() const { glUseProgram(ID); }
void Shader::unbind() const { glUseProgram(0); }

// Set Bool uniform
void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

// Set Int uniform
void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

// Set uInt uniform
void Shader::setUInt(const std::string &name, int value) const {
    glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
}

// Set Float uniform
void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// Set Vec2 uniform
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1,
                 glm::value_ptr(value));
}

// Set Vec3 uniform
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1,
                 glm::value_ptr(value));
}

// Set Mat4 uniform
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       glm::value_ptr(mat));
}

// Recursively create an "expanded" shader code string (expanding #include
// statments) in the code
std::string Shader::getExpandedShaderCode(const std::string &shaderPath) const {

    // Get the file and check if it exists
    std::ifstream file(shaderPath);
    if (!file.is_open()) {
        Logger::fatal("SHADER", "ERROR::SHADER::FILE_NOT_FOUND: " + shaderPath);
        return "";
    }

    // Get the directory of the current file to resolve relative includes
    fs::path currentFilePath(shaderPath);
    fs::path directory = currentFilePath.parent_path();

    // Save variables for the output & line
    std::stringstream output;
    std::string line;

    // Loop until the end of the shader file
    while (std::getline(file, line)) {
        // Look for #include "filename"
        if (line.find("#include") == 0) {

            // Find the first & last quotation mark of the include statements
            size_t firstQuote = line.find('\"');
            size_t lastQuote = line.find_last_of('\"');

            // Check to make sure that they were found by .find()
            if (firstQuote != std::string::npos &&
                lastQuote != std::string::npos && firstQuote != lastQuote) {

                // Get the file name of the #include statement
                std::string includeFileName =
                    line.substr(firstQuote + 1, lastQuote - firstQuote - 1);

                // Construct path relative to the current shader file
                fs::path includePath = directory / includeFileName;

                // Recursively process the included file + add logged comments
                output << "// BEGIN INCLUDE: " << includeFileName << "\n";
                output << getExpandedShaderCode(includePath.string());
                output << "// END INCLUDE: " << includeFileName << "\n";

                // Continue the while-loop (skipping the concatenation)
                continue;
            }
        }
        output << line << "\n";
    }

    // Return the final string
    return output.str();
}

// Output the final shader code to a dump file for debug purposes
void Shader::dumpExpandedShaderCode(const std::string &sourceFileName,
                                    const std::string &source,
                                    const std::string &type) const {

    // Creates a directory called 'debug_shaders' if it doesn't exist
    if (!fs::exists("debugShaders")) {
        fs::create_directory("debugShaders");
    }

    // Get the file name and an out stream
    std::string fileName =
        "debugShaders/last_" + type + "_" + sourceFileName + ".glsl";
    std::ofstream out(fileName);

    // Open the file, push the source code, and close it
    if (out.is_open()) {
        out << source;
        out.close();
    }
}

// Custom compilation error checking script
void Shader::checkCompileErrors(const uint32_t shader,
                                const std::string type) const {
    // Satus code and info log
    int success;
    char infoLog[1024];

    // Not program compilation error (VERTEX, FRAGMENT, COMPUTE)
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            Logger::fatal(
                "SHADER",
                "ERROR::SHADER_COMPILATION_ERROR of type: " + type + "\n" +
                    infoLog +
                    "\n -- "
                    "--------------------------------------------------- "
                    "-- ");
        }
    }

    // Program compilation error (PROGRAM)
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            Logger::fatal(
                "SHADER",
                "ERROR::PROGRAM_LINKING_ERROR of type: " + type + "\n" +
                    infoLog +
                    "\n -- "
                    "--------------------------------------------------- "
                    "-- ");
        }
    }
}

} // namespace Engine
