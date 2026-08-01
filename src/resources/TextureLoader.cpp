#include "TextureLoader.h"

#include "../services/Logger.h"
#include "CPUStructs.h"

#include <cstddef>
#include <glm/glm.hpp>
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine {

std::shared_ptr<CPUTextureData>
TextureLoader::loadTexture(const std::string &filepath) {

    std::shared_ptr<CPUTextureData> texture;

    // Set stb flags
    stbi_set_flip_vertically_on_load(true);

    // Load the image
    int width, height, channels;
    unsigned char *data =
        stbi_load(filepath.c_str(), &width, &height, &channels, 0);

    // Check for a sucessful load
    if (!data) {
        Logger::error("ASSET", "Failed to load texture at: " + filepath);
        return nullptr;
    }

    // Free stb image data
    stbi_image_free(data);

    texture->width = width;
    texture->height = height;
    texture->channels = channels;

    // Get the GL image format
    texture->format = (channels == 1)   ? GL_RED
                      : (channels == 4) ? GL_RGBA
                                        : GL_RGB;
    texture->pixels =
        std::vector<unsigned char>(data, data + width * height * channels);

    Logger::info("ASSET", "Successfully loaded texture at: " + filepath);
    return texture;
}
} // namespace Engine
