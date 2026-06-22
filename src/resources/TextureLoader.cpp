#include "TextureLoader.h"

#include "../services/Logger.h"

#define STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <stb_image.h>

namespace Engine {

GLuint TextureLoader::loadTexture(const std::string &filepath,
                                  bool &bumpTexture) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;
    unsigned char *data =
        stbi_load(filepath.c_str(), &width, &height, &channels, 0);
    if (!data) {
        Logger::error("ASSET", "Failed to load texture at: " + filepath);
        return 0;
    }

    const GLenum format = (channels == 1)   ? GL_RED
                          : (channels == 4) ? GL_RGBA
                                            : GL_RGB;
    bumpTexture = channels == 1;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    Logger::info("ASSET", "Successfully loaded texture at: " + filepath);
    return textureID;
}
} // namespace Engine
