#include "IProgram.h"

#include "../../services/Logger.h"
#include "IShader.h"
#include <glm/gtc/type_ptr.hpp>

namespace Engine {

IProgram::IProgram() : ID(0) { ID = glCreateProgram(); }

void IProgram::attachShader(IShader shader) {
    glAttachShader(ID, shader.glSourceShader);
    attachedShaders.push_back(shader);
}

void IProgram::link() {
    glLinkProgram(ID);
    checkLinkErrors();

    attachedShaders.clear();
}

// Bind and unbind
void IProgram::bind() const { glUseProgram(ID); }
void IProgram::unbind() const { glUseProgram(0); }

// Set Bool uniform
void IProgram::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

// Set Int uniform
void IProgram::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

// Set uInt uniform
void IProgram::setUInt(const std::string &name, int value) const {
    glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
}

// Set Float uniform
void IProgram::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

// Set Vec2 uniform
void IProgram::setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1,
                 glm::value_ptr(value));
}

// Set Vec3 uniform
void IProgram::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1,
                 glm::value_ptr(value));
}

// Set Mat4 uniform
void IProgram::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       glm::value_ptr(mat));
}

void IProgram::checkLinkErrors() const {
    // Satus code and info log
    int success;
    char infoLog[1024];

    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    // Not program compilation error (VERTEX, FRAGMENT, COMPUTE)
    if (!success) {
        glGetProgramInfoLog(ID, 1024, NULL, infoLog);
        Logger::fatal("SHADER",
                      "ERROR::PROGRAM_LINKING_ERROR\n" + std::string(infoLog) +
                          "\n -- "
                          "--------------------------------------------------- "
                          "-- ");
    }
}

} // namespace Engine
