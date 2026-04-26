#include "Shader.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

// On construction - compile source code & program
Shader::Shader(const char *vertexPath, const char *fragmentPath) {

    // Set spots for the code & open file streams
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::string vertexCode;
    std::string fragmentCode;

    // Set for file exceptions
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    // Try-catch loop for shader file opening and parsing
    try {

        // Open the shader files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        // Create and fill file reading streams
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // Close code files
        vShaderFile.close();
        fShaderFile.close();

        // Turn the streams to strings and set the code
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure &e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    // Get char pointers for the strings
    const char *vShaderSource = vertexCode.c_str();
    const char *fShaderSource = fragmentCode.c_str();

    // Compile the shaders
    uint32_t vertex, fragment;

    // Create a vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);

    // Set the shader code and compile it
    glShaderSource(vertex, 1, &vShaderSource, NULL);
    glCompileShader(vertex);

    // Check for compilation errors
    checkCompileErrors(vertex, "VERTEX");

    // Create a fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);

    // Set the shader code and compile it
    glShaderSource(fragment, 1, &fShaderSource, NULL);
    glCompileShader(fragment);

    // Check for compilation errors
    checkCompileErrors(fragment, "FRAGMENT");

    // Create an OpenGL program
    ID = glCreateProgram();

    // Attatch the vert & fragment shaders
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

// Set Mat4 uniform
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       glm::value_ptr(mat));
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
