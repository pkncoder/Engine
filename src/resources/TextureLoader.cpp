#include "TextureLoader.h"

#include "../services/Logger.h"

#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine {

std::shared_ptr<CPUTextureData>
TextureLoader::loadTexture(const std::string &sourceDirectory,
                           const std::string &filename) {

    std::string expectedPath = sourceDirectory + filename;

    // Final texture data
    std::shared_ptr<CPUTextureData> texture =
        std::make_shared<CPUTextureData>();

    // Set stb flags
    stbi_set_flip_vertically_on_load(true);

    // Load the image
    int width, height, channels;
    unsigned char *data =
        stbi_load(expectedPath.c_str(), &width, &height, &channels, 0);

    // Check for a sucessful load
    if (!data) {
        Logger::error("ASSET", "Failed to load texture at: " + expectedPath);
        return nullptr;
    }

    // Set image size values to the texture
    texture->width = width;
    texture->height = height;
    texture->channels = channels;

    // TODO: format

    // Convert the data to a vector of chars for the pixels
    texture->pixels =
        std::vector<unsigned char>(data, data + width * height * channels);

    // Free stb image data
    stbi_image_free(data);

    Logger::info("ASSET", "Successfully loaded texture at: " + expectedPath);
    return texture;
}
} // namespace Engine
