#include "IRenderer.h"

#include "../services/UUID.h"
#include <cstdint>

namespace Engine {

void IRenderer::shutdown() {
    for (auto &[handle, target] : renderTargets) {
        if (target.id != 0) {
            glDeleteTextures(1, &target.id);
            target.id = 0;
        }
    }

    renderTargets.clear();
    renderTargetNameMap.clear();
    shaderPasses.clear();
}

void IRenderer::resize(const uint32_t newWidth, const uint32_t newHeight) {
    if (newWidth == currentWidth && newHeight == currentHeight) {
        return;
    }

    if (newWidth == 0 || newHeight == 0) {
        return;
    }

    currentWidth = newWidth;
    currentHeight = newHeight;

    for (auto &[handle, target] : renderTargets) {
        allocateRenderTarget(target);
    }
}

RenderTargetHandle IRenderer::addRenderTarget(const std::string &name,
                                              const GLuint bindingIndex,
                                              const GLenum format) {
    // Create a new render target & set attributes
    RenderTarget target;
    target.name = name;
    target.handle = UUID();
    target.bindingIndex = bindingIndex;
    target.format = format;

    // Check for size issues & alocate/bind the target
    if (currentWidth > 0 && currentHeight > 0) {
        allocateRenderTarget(target);
        bindRenderTarget(target);
    }

    // Save the target to the registry
    renderTargets[target.handle] = target;

    return target.handle;
}

ShaderPass &IRenderer::addShaderPass(const std::string &name,
                                     const char *computeShaderPath,
                                     const bool enabled) {
    // Create a new shader pass & set attributes
    ShaderPass pass;
    pass.name = name;
    pass.shader = Shader(computeShaderPath);
    pass.enabled = enabled;

    // Add the shader pass to the registry
    shaderPasses.push_back(std::move(pass));
    return shaderPasses.back();
}

ShaderPass &IRenderer::addShaderPass(const std::string &name,
                                     const char *vertexShaderPath,
                                     const char *fragmentShaderPath,
                                     const bool enabled) {
    // Create a new shader pass & set attributes
    ShaderPass pass;
    pass.name = name;
    pass.shader = Shader(vertexShaderPath, fragmentShaderPath);
    pass.enabled = enabled;

    // Add the shader pass to the registry
    shaderPasses.push_back(std::move(pass));
    return shaderPasses.back();
}

RenderTarget *IRenderer::getRenderTarget(const RenderTargetHandle handle) {
    auto it = renderTargets.find(handle);
    return it != renderTargets.end() ? &it->second : nullptr;
}

RenderTarget *IRenderer::getRenderTargetByName(const std::string &name) {
    auto it = renderTargetNameMap.find(name);
    if (it != renderTargetNameMap.end()) {
        return &renderTargets[it->second];
    }
    return nullptr;
}

void IRenderer::allocateRenderTarget(RenderTarget &target) const {
    // If the texture id exists, delete it
    if (target.id != 0) {
        glDeleteTextures(1, &target.id);
    }

    // Create a texture and bind it for modification
    glGenTextures(1, &target.id);
    glBindTexture(GL_TEXTURE_2D, target.id);

    // Set texture params
    glTexImage2D(GL_TEXTURE_2D, 0, target.format, currentWidth, currentHeight,
                 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void IRenderer::bindRenderTarget(RenderTarget &target) const {
    // Bind the texture to it's binding index
    glBindImageTexture(target.bindingIndex, target.id, 0, GL_FALSE, 0,
                       GL_WRITE_ONLY, target.format);
};

} // namespace Engine
