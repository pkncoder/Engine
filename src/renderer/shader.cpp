#include "Shader.h"
#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
  // 1. Retrieve the source code from file paths
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;

  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    vShaderFile.close();
    fShaderFile.close();
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  } catch (std::ifstream::failure &e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
  }

  const char *vShaderSource = vertexCode.c_str();
  const char *fShaderSource = fragmentCode.c_str();

  // 2. Compile shaders
  uint32_t vertex, fragment;

  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderSource, NULL);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "VERTEX");

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderSource, NULL);
  glCompileShader(fragment);
  checkCompileErrors(fragment, "FRAGMENT");

  // 3. Shader Program
  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);
  checkCompileErrors(ID, "PROGRAM");

  // Delete shaders as they're linked into our program and no longer necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::Bind() const { glUseProgram(ID); }
void Shader::Unbind() const { glUseProgram(0); }

void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(mat));
}

// Implement SetInt, SetFloat, etc., similarly using glUniform...

void Shader::checkCompileErrors(uint32_t shader, std::string type) {
  int success;
  char infoLog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::cout
          << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
          << infoLog
          << "\n -- --------------------------------------------------- -- "
          << std::endl;
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::cout
          << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
          << infoLog
          << "\n -- --------------------------------------------------- -- "
          << std::endl;
    }
  }
}
