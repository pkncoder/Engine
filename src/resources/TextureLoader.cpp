#include "TextureLoader.h"
#include "../services/Logger.h"

#define STB_IMAGE_IMPLEMENTATION // Uncomment this if not defined elsewhere!
#include <stb_image.h>

namespace Engine {

GLuint TextureLoader::loadTexture(const std::string &filepath) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, channels;
    // Flip textures vertically so OpenGL reads the UVs correctly (0,0 at bottom
    // left)
    stbi_set_flip_vertically_on_load(true);

    unsigned char *data =
        stbi_load(filepath.c_str(), &width, &height, &channels, 0);
    if (data) {
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Standard wrapping and filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        Logger::info("ASSET", "Successfully loaded texture at: " + filepath);
    } else {
        Logger::error("ASSET", "Failed to load texture at: " + filepath);
        stbi_image_free(data);
        return 0; // Return 0 to indicate failure
    }

    return textureID;
}

} // namespace Engine
