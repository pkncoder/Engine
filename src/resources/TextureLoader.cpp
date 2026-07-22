#include "TextureLoader.h"

#include "../services/Logger.h"

#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine {

GLuint TextureLoader::loadTexture(const std::string &filepath,
                                  bool &isBumpTexture) {
    // Generate a texture id
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Set stb flags
    stbi_set_flip_vertically_on_load(true);

    // Load the image
    int width, height, channels;
    unsigned char *data =
        stbi_load(filepath.c_str(), &width, &height, &channels, 0);

    // Check for a sucessful load
    if (!data) {
        Logger::error("ASSET", "Failed to load texture at: " + filepath);
        return 0;
    }

    // Get the GL image format
    const GLenum format = (channels == 1)   ? GL_RED
                          : (channels == 4) ? GL_RGBA
                                            : GL_RGB;
    isBumpTexture = channels == 1;

    // Bind the opengl texture & set alignment + data
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D); // Generate the LODs

    // Texture params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Free stb image data
    stbi_image_free(data);

    Logger::info("ASSET", "Successfully loaded texture at: " + filepath);
    return textureID;
}
} // namespace Engine
