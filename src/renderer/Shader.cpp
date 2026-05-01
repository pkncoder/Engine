#include "Shader.h"

#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

namespace Engine {

namespace fs = std::filesystem;

// On construction - compile source code & program
Shader::Shader(const char *vertexPath, const char *fragmentPath) {

    std::string vertexCode = processIncludes(vertexPath);
    std::string fragmentCode = processIncludes(fragmentPath);

    dumpLog(vertexCode, "VERTEX");
    dumpLog(fragmentCode, "FRAGMENT");

    // Get char pointers for the strings
    const char *vShaderSource = vertexCode.c_str();
    const char *fShaderSource = fragmentCode.c_str();

    // Compile the shaders
    uint32_t vertex, fragment;

    // Create a vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderSource, NULL);
    glCompileShader(vertex);

    // Check for compilation errors
    checkCompileErrors(vertex, "VERTEX");

    // Create a fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderSource, NULL);
    glCompileShader(fragment);

    // Check for compilation errors
    checkCompileErrors(fragment, "FRAGMENT");

    // Create an OpenGL program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    // Link the program to OpenGL
    glLinkProgram(ID);

    // Check for projgram creation errors
    checkCompileErrors(ID, "PROGRAM");

    // Delete shaders as they're linked into our program and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
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

// Set Float uniform
void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
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

std::string Shader::processIncludes(const std::string &shaderPath) {

    // Get the file and check if it exists
    std::ifstream file(shaderPath);
    if (!file.is_open()) {
        std::cerr << "ERROR::SHADER::FILE_NOT_FOUND: " << shaderPath
                  << std::endl;
        return "";
    }

    // Get the directory of the current file to resolve relative includes
    fs::path currentFilePath(shaderPath);
    fs::path directory = currentFilePath.parent_path();

    std::stringstream output;
    std::string line;

    while (std::getline(file, line)) {
        // Look for #include "filename"
        if (line.find("#include") == 0) {
            size_t firstQuote = line.find('\"');
            size_t lastQuote = line.find_last_of('\"');

            if (firstQuote != std::string::npos &&
                lastQuote != std::string::npos && firstQuote != lastQuote) {
                std::string includeFileName =
                    line.substr(firstQuote + 1, lastQuote - firstQuote - 1);

                // Construct path relative to the current shader file
                fs::path includePath = directory / includeFileName;

                // Recursively process the included file
                output << "// BEGIN INCLUDE: " << includeFileName << "\n";
                output << processIncludes(includePath.string());
                output << "// END INCLUDE: " << includeFileName << "\n";
                continue;
            }
        }
        output << line << "\n";
    }

    return output.str();
}

void Shader::dumpLog(const std::string &source, const std::string &type) {
    // Creates a directory called 'debug' if it doesn't exist
    if (!fs::exists("debug_shaders")) {
        fs::create_directory("debug_shaders");
    }

    std::string fileName = "debug_shaders/last_" + type + ".glsl";
    std::ofstream out(fileName);

    if (out.is_open()) {
        out << source;
        out.close();
    }
}

// Custom compilation error checking script
void Shader::checkCompileErrors(uint32_t shader, std::string type) const {
    // Satus code and info log
    int success;
    char infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout
                << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                << infoLog
                << "\n -- --------------------------------------------------- "
                   "-- "
                << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout
                << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                << infoLog
                << "\n -- --------------------------------------------------- "
                   "-- "
                << std::endl;
        }
    }
}

} // namespace Engine
