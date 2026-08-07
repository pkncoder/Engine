#include "TextureLoader.h"

#include "../services/Logger.h"

#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine {

std::shared_ptr<CPUTextureData>
TextureLoader::loadTexture(const std::string &filepath) {

    // Final texture data
    std::shared_ptr<CPUTextureData> texture =
        std::make_shared<CPUTextureData>();

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

    // Set image size values to the texture
    texture->width = width;
    texture->height = height;
    texture->channels = channels;

    // TODO: format

    // Convert the data to a vector of chars for the pixels
    texture->pixels =
        std::vector<unsigned char>(data, data + width * height * channels);

    Logger::info("ASSET", "Successfully loaded texture at: " + filepath);
    return texture;
}
} // namespace Engine
