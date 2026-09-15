#include "IRenderer.h"

#include "../services/Logger.h"
#include "../services/UUIDGenerator.h"

namespace Engine {

void IRenderer::shutdown() {

    clearAllRenderTargets();
    // shaderNodeTree.clear();
}

void IRenderer::clearAllRenderTargets() {
    for (auto &[handle, target] : renderTargets) {
        for (auto &id : target.textureIDs) {
            if (id != 0) {
                glDeleteTextures(1, &id);
                id = 0;
            }
        }

        if (target.depthTextureID != 0) {
            glDeleteTextures(1, &target.depthTextureID);
            target.depthTextureID = 0;
        }

        if (target.fbo != 0) {
            glDeleteFramebuffers(1, &target.fbo);
            target.fbo = 0;
        }
    }

    renderTargets.clear();
    renderTargetNameMap.clear();
}

void IRenderer::resize(const uint32_t newWidth, const uint32_t newHeight) {

    if (newWidth == currentWidth && newHeight == currentHeight) {
        return;
    }

    // Check for invalid size
    if (newWidth == 0 || newHeight == 0) {
        return;
    }

    // Set new width & height
    currentWidth = newWidth;
    currentHeight = newHeight;

    // // Reset each texture for the render targets
    // for (auto &[handle, target] : renderTargets) {
    //     allocateRenderTarget(target);
    // }
}

RenderTargetHandle
IRenderer::addRenderTarget(const std::string &name,
                           const std::vector<GLenum> &formats,
                           const bool createDepth) {
    RenderTarget renderTarget;
    renderTarget.name = name;
    renderTarget.handle = UUIDGenerator::generate();

    // 2. Generate and bind the Framebuffer
    glGenFramebuffers(1, &renderTarget.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, renderTarget.fbo);

    std::vector<GLenum> drawBuffers;

    // 3. Loop through formats and create Multiple Render Targets (MRT)
    for (size_t i = 0; i < formats.size(); ++i) {
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        // Use the internal format passed in (e.g., GL_RGBA16F, GL_RGB8)
        // Assuming format is floating point for G-Buffer data. You might need
        // to split internal format and format if your setup requires it.
        glTexImage2D(GL_TEXTURE_2D, 0, formats[i], currentWidth, currentHeight,
                     0, GL_RGBA, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Attach to GL_COLOR_ATTACHMENT0 + i
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                               GL_TEXTURE_2D, tex, 0);

        renderTarget.textureIDs.push_back(tex);
        drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
    }

    // Tell OpenGL which color attachments we'll use for rendering
    if (!drawBuffers.empty()) {
        glDrawBuffers(drawBuffers.size(), drawBuffers.data());
    }

    // 4. Handle Depth Buffer Flag
    if (createDepth) {
        glGenRenderbuffers(1, &renderTarget.depthTextureID);
        glBindRenderbuffer(GL_RENDERBUFFER, renderTarget.depthTextureID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
                              currentWidth, currentHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER, renderTarget.depthTextureID);
    } else {
        renderTarget.depthTextureID = 0;
    }

    // 5. Check if the FBO is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        // Log your error here (e.g., std::cerr << "Framebuffer not complete!"
        // << std::endl;)
    }

    // Unbind to prevent accidental modifications
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 6. Cache and return
    renderTargets[renderTarget.handle] = renderTarget;

    return renderTarget.handle;
}

// void IRenderer::setDisplayTarget(RenderTargetHandle handle) {
//     if (renderTargets.find(handle) == renderTargets.end()) {
//         Logger::warn("RENDERER", "Attempted to set invalid display target: "
//         +
//                                      std::to_string(handle));
//         return;
//     }
//
//     // Set the name for later fetching
//     currentRenderTarget = handle;
// }

RenderTarget *IRenderer::getRenderTarget(const RenderTargetHandle handle) {
    // Find the render target via handle, if it exists return a refrance
    auto ittr = renderTargets.find(handle);
    return ittr != renderTargets.end() ? &ittr->second : nullptr;
}

RenderTarget *IRenderer::getRenderTargetByName(const std::string &name) {
    // Find the render target via name, if it exists return a refrance
    auto ittr = renderTargetNameMap.find(name);
    return ittr != renderTargetNameMap.end() ? &renderTargets[ittr->second]
                                             : nullptr;
}

// void IRenderer::allocateRenderTarget(RenderTarget &target) const {
//     // If the texture id exists, delete it
//     if (target.id != 0) {
//         glDeleteTextures(1, &target.id);
//     }
//
//     // Create a texture and bind it for modification
//     glGenTextures(1, &target.id);
//     glBindTexture(GL_TEXTURE_2D, target.id);
//
//     // Set texture params
//     glTexImage2D(GL_TEXTURE_2D, 0, target.format, currentWidth,
//     currentHeight,
//                  0, GL_RGBA, GL_FLOAT, NULL);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//     // Unbind the texture
//     glBindTexture(GL_TEXTURE_2D, 0);
// }
//
// void IRenderer::bindRenderTarget(RenderTarget &target) const {
//     // Bind the texture to it's binding index
//     glBindImageTexture(target.bindingIndex, target.id, 0, GL_FALSE, 0,
//                        GL_WRITE_ONLY, target.format);
// }

void IRenderer::drawFullscreenQuad() {
    // Lazy initialization of the quad VAO/VBO
    if (fullscreenQuadVAO == 0) {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
        };

        glGenVertexArrays(1, &fullscreenQuadVAO);
        glGenBuffers(1, &fullscreenQuadVAO);
        glBindVertexArray(fullscreenQuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, fullscreenQuadVAO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                     GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                              (void *)0);
        // Texture coordinate attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                              (void *)(3 * sizeof(float)));
    }

    // Draw the quad
    glBindVertexArray(fullscreenQuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

} // namespace Engine
