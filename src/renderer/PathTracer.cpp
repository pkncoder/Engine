#include "PathTracer.h"

#include "../resources/AssetManager.h"
#include "../scene/components/MaterialComponent.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Logger.h"
#include "BufferManager.h"

#include <set>

namespace Engine {

// --- INITIALIZATION & SHUTDOWN ---

// Checking for system compatibility & doing setup
void PathTracer::init() {

    // Check for compute shader support, and throw an error + exit if not
    // supported
    if (glDispatchCompute == nullptr) {
        Logger::fatal("RENDERER", "Compute Shaders are not supported!");
        return;
    }

    glGenFramebuffers(1, &presentFBO);

    const size_t MAX_INSTANCES = 10000; // TODO: renderer settings maybe?
    addStorageBuffer("meshEntries", 0, sizeof(GPUMeshEntry),
                     1024);                                   // Mesh Entries
    addStorageBuffer("vertices", 1, sizeof(GPUVertex), 1024); // Vertices
    addStorageBuffer("indices", 2, sizeof(uint32_t), 1024);   // Indices
    addStorageBuffer("instances", 3, sizeof(GPUInstance), MAX_INSTANCES);
    addStorageBuffer("materials", 4, sizeof(GPUMaterial), MAX_INSTANCES);

    addRenderTarget("MainColorOutput", 0);

    addShaderPass("renderPass", "shaders/compute/pathTracer.comp");
    addShaderPass("invert", "shaders/compute/invert.comp", false);

    setDisplayTarget("MainColorOutput"); // Display the main output by default

    Logger::info("RENDERER", "Path Tracer initialized");
}

// Clean up
void PathTracer::shutdown() {

    // Clean up the buffers
    for (auto &[name, buffer] : storageBuffers) {
        buffer.shutdown();
    }
    storageBuffers.clear();

    // Clean up the textures
    for (auto &[name, target] : renderTargets) {
        if (target.id != 0)
            glDeleteTextures(1, &target.id);
    }
    renderTargets.clear();
}

// --- Rendering & render management ---

// Run a full render pass
void PathTracer::render(const Camera &camera, Scene &scene, float aspectRatio) {

    // Check no texture size
    if (currentWidth == 0 || currentHeight == 0)
        return;

    // Increase the tracked frame count
    frameCount++;

    // Get the data flattened for the buffers
    // TODO: Rework
    flattenScene(scene);

    // Loop each shader pass
    for (auto &pass : shaderPasses) {
        if (!pass.enabled) // If the pass isn't enabled, skip it
            continue;

        // Bind the shader & global uniforms to it
        pass.shader.bind();
        bindGlobalUniforms(pass.shader, camera);

        // Dispatch the shader pass
        dispatchShaderPass(pass);
    }
}

// Resize the render
void PathTracer::resize(int newWidth, int newHeight) {

    // If the new size is the same as the old one, do nothing
    if (newWidth == currentWidth && newHeight == currentHeight)
        return;

    // Update the tracked size
    currentWidth = newWidth;
    currentHeight = newHeight;

    // Rebuild each render target
    for (auto &[name, target] : renderTargets) {

        // TODO: vice-versa?
        if (target.id != 0)
            glDeleteTextures(1, &target.id);

        // Allocate and bind the render target's texture w/ the new size
        allocateRenderTarget(target);
        bindRenderTarget(target);
    }
}

// --- Render target presenting ---

// Set the display target based on it's name
void PathTracer::setDisplayTarget(const std::string &name) {

    // Check for the passes's existance
    if (renderTargets.find(name) == renderTargets.end()) {
        Logger::warn("RENDERER",
                     "Attempted to set invalid display target: " + name);
        return;
    }

    // Set the name for later fetching
    currentRenderTarget = name;
}

// Blit the chosen render target to the fbo for presenting
void PathTracer::present(int width, int height) {

    // Bind the framebuffer for use
    glBindFramebuffer(GL_READ_FRAMEBUFFER, presentFBO);

    // Find the render target for use
    const auto ittr = renderTargets.find(currentRenderTarget);

    // Make sure the target exists
    if (ittr == renderTargets.end()) {
        Logger::error("RENDERER", "Could not find the render target.");
        return;
    }

    // Get the texture from the render target
    const GLuint targetTex = ittr->second.id;

    // Set the fbo's texture to the render target's
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, targetTex, 0);

    // Blit the framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

// --- Dynamic RESOURCE MANAGEMENT ---

void PathTracer::addStorageBuffer(const std::string &name, GLuint bindingIndex,
                                  size_t elementSize,
                                  size_t initialElementCount) {
    PersistentBuffer newBuffer;
    newBuffer.bindingIndex = bindingIndex;
    newBuffer.elementSize = elementSize;

    newBuffer.setup(GL_SHADER_STORAGE_BUFFER,
                    elementSize * initialElementCount);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, newBuffer.id);

    storageBuffers[name] = newBuffer;
}

void PathTracer::addRenderTarget(const std::string &name, GLuint bindingIndex,
                                 GLenum format) {
    RenderTarget target;
    target.name = name;
    target.bindingIndex = bindingIndex;
    target.format = format;

    if (currentWidth > 0 && currentHeight > 0) {
        allocateRenderTarget(target);
        bindRenderTarget(target);
    }

    renderTargets[name] = target;
}

void PathTracer::addShaderPass(const std::string &name,
                               const char *computeShaderPath,
                               const bool enabled) {
    ShaderPass pass;
    pass.name = name;
    pass.shader = Shader(computeShaderPath);
    pass.enabled = enabled;

    shaderPasses.push_back(std::move(pass));
}

// --- Render target management ---

void PathTracer::allocateRenderTarget(RenderTarget &target) {
    if (target.id != 0) {
        glDeleteTextures(1, &target.id);
    }

    glGenTextures(1, &target.id);
    glBindTexture(GL_TEXTURE_2D, target.id);
    glTexImage2D(GL_TEXTURE_2D, 0, target.format, currentWidth, currentHeight,
                 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void PathTracer::bindRenderTarget(RenderTarget &target) {
    glBindImageTexture(target.bindingIndex, target.id, 0, GL_FALSE, 0,
                       GL_WRITE_ONLY, target.format);
}

// -- Buffer management

void PathTracer::updateBuffer(const std::string &name, const void *data,
                              size_t elementCount) {
    if (elementCount == 0)
        return;

    const auto ittr = storageBuffers.find(name);
    if (ittr == storageBuffers.end()) {
        Logger::error("RENDERER",
                      "PathTracer::updateBuffer - unknown buffer: " + name);
        return;
    }

    PersistentBuffer &buffer = ittr->second;

    buffer.update(data, elementCount * buffer.elementSize);
}

// --- Scene managemnet ---

void PathTracer::flattenScene(Scene &activeScene) {
    if (geometryDirty) {
        rebuildGeometryLookupTable(activeScene);
    }

    const auto renderables =
        activeScene.getMatchingEntities<TransformComponent, MeshComponent,
                                        MaterialComponent>();

    for (EntityID id : renderables) {
        auto &meshComp = activeScene.getComponent<MeshComponent>(id);
        if (instanceLookupTable.find(meshComp.assetID) ==
            instanceLookupTable.end()) {
            geometryDirty = true;
            break;
        }
    }

    instances.clear();
    instances.reserve(renderables.size());

    // TODO: Material dirty flag
    materialList.clear();
    materialList.reserve(renderables.size());

    for (EntityID id : renderables) {
        auto &meshComp = activeScene.getComponent<MeshComponent>(id);
        auto &transform = activeScene.getComponent<TransformComponent>(id);
        auto &materialComp = activeScene.getComponent<MaterialComponent>(id);

        GPUInstance inst{};

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
        model *= glm::mat4_cast(transform.rotation);
        model = glm::scale(model, transform.scale);

        inst.transform = model;
        inst.invTransform = glm::inverse(model);
        inst.meshIndex = instanceLookupTable[meshComp.assetID];
        // TODO: materialID

        instances.push_back(inst);

        GPUMaterial mat{};
        mat.albedo = glm::vec4(materialComp.albedo, 0.0f);
        mat.emmissive = glm::vec4(materialComp.emmissive, 0.0f);
        mat.roughness = materialComp.roughness;
        mat.metallic = materialComp.metallic;

        materialList.push_back(mat);
    }

    updateBuffer("instances", instances.data(), instances.size());
    updateBuffer("materials", materialList.data(), materialList.size());
}

void PathTracer::rebuildGeometryLookupTable(Scene &activeScene) {
    std::vector<GPUVertex> instanceVertices;
    std::vector<uint32_t> instanceIndices;
    std::vector<GPUMeshEntry> meshEntries;

    instanceLookupTable.clear();

    std::set<std::string> uniqueMeshes;
    auto renderables = activeScene.getMatchingEntities<MeshComponent>();
    for (EntityID id : renderables) {
        uniqueMeshes.insert(
            activeScene.getComponent<MeshComponent>(id).assetID);
    }

    for (const auto &assetID : uniqueMeshes) {
        auto meshData = AssetManager::loadMesh(assetID);

        GPUMeshEntry entry{};
        entry.baseVertex = static_cast<uint32_t>(instanceVertices.size());
        entry.baseIndex = static_cast<uint32_t>(instanceIndices.size());
        entry.indexCount = static_cast<uint32_t>(meshData->indices.size());

        for (const auto &v : meshData->vertices) {
            instanceVertices.push_back({glm::vec4(v.position, 1.0f),
                                        glm::vec4(v.normal, 0.0f),
                                        glm::vec4(v.texCoords, 0.0f, 0.0f)});
        }

        instanceIndices.insert(instanceIndices.end(), meshData->indices.begin(),
                               meshData->indices.end());

        instanceLookupTable[assetID] =
            static_cast<uint32_t>(meshEntries.size());
        meshEntries.push_back(entry);
    }

    updateBuffer("meshEntries", meshEntries.data(), meshEntries.size());
    updateBuffer("vertices", instanceVertices.data(), instanceVertices.size());
    updateBuffer("indices", instanceIndices.data(), instanceIndices.size());

    geometryDirty = false;
}

// --- Uniforms ---

void PathTracer::bindGlobalUniforms(Shader &shader, const Camera &camera) {
    // TODO: UBO for at least defaults & dynamic uniforms
    shader.setInt("u_frameNum", frameCount);
    shader.setVec3("u_cameraPos", camera.position);
    shader.setFloat("u_FOV", camera.fov);
    shader.setInt("u_instanceCount", static_cast<int>(instances.size()));
    shader.setMat4("u_inverseView", glm::inverse(camera.getViewMatrix()));
}

// --- Shader pass management ---

void PathTracer::dispatchShaderPass(const ShaderPass &pass) {

    const GLuint groupsX =
        (currentWidth + pass.workgroupSize.x - 1) / pass.workgroupSize.x;
    const GLuint groupsY =
        (currentHeight + pass.workgroupSize.y - 1) / pass.workgroupSize.y;
    // TODO: groupsZ

    glDispatchCompute(groupsX, groupsY, pass.workgroupSize.z);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
                    GL_SHADER_STORAGE_BARRIER_BIT | GL_FRAMEBUFFER_BARRIER_BIT);
}

} // namespace Engine
