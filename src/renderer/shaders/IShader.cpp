#include "IShader.h"

#include "../../services/Logger.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace Engine {

namespace fs = std::filesystem;

std::string to_string(ShaderType type) {
    switch (type) {
    case ShaderType::Vertex:
        return "VERTEX";
    case ShaderType::Geometry:
        return "GEOMETRY";
    case ShaderType::Fragment:
        return "FRAGMENT";
    case ShaderType::Compute:
        return "COMPUTE";
    }

    return "UNKNOWN_SHADER_TYPE";
}

IShader::IShader(const char *_sourcePath, const ShaderType _type)
    : sourcePath(_sourcePath), type(_type) {

    sourceFilename = std::string(sourcePath)
                         .substr(std::string(sourcePath).find_last_of('/') + 1);

    source = getExpandedShaderCode(sourcePath);

    dumpExpandedShaderCode();

    const char *sourceCStr = source.c_str();

    glSourceShader = glCreateShader(type);
    glShaderSource(glSourceShader, 1, &sourceCStr, NULL);
    glCompileShader(glSourceShader);

    checkCompileErrors();
}

IShader::~IShader() {
    if (glSourceShader != 0)
        glDeleteShader(glSourceShader);
}

// Recursively create an "expanded" shader code string (expanding #include
// statments) in the code
std::string
IShader::getExpandedShaderCode(const std::string &shaderPath) const {

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
void IShader::dumpExpandedShaderCode() const {

    // Creates a directory called 'debug_shaders' if it doesn't exist
    if (!fs::exists("debugShaders")) {
        fs::create_directory("debugShaders");
    }

    // Get the file name and an out stream
    std::string fileName =
        "debugShaders/last_" + to_string(type) + "_" + sourceFilename + ".glsl";
    std::ofstream out(fileName);

    // Open the file, push the source code, and close it
    if (out.is_open()) {
        out << source;
        out.close();
    }
}

// Custom compilation error checking script
void IShader::checkCompileErrors() const {
    // Satus code and info log
    int success;
    char infoLog[1024];

    glGetShaderiv(glSourceShader, GL_COMPILE_STATUS, &success);

    // Not program compilation error (VERTEX, FRAGMENT, COMPUTE)
    if (!success) {
        glGetShaderInfoLog(glSourceShader, 1024, NULL, infoLog);
        Logger::fatal("SHADER",
                      "ERROR::SHADER_COMPILATION_ERROR of type: " +
                          to_string(type) + "\n" + infoLog +
                          "\n -- "
                          "--------------------------------------------------- "
                          "-- ");
    }
}

} // namespace Engine
