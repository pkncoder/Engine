#include "PathTracer.h"

#include "../Constants.h"
#include "../scene/SceneManager.h"
#include "../scene/components/MaterialComponent.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Logger.h"
#include "BufferManager.h"

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

    addStorageBuffer("meshEntries", 0, sizeof(GPUMeshEntry),
                     1024);                                   // Mesh Entries
    addStorageBuffer("vertices", 1, sizeof(GPUVertex), 1024); // Vertices
    addStorageBuffer("indices", 2, sizeof(uint32_t), 1024);   // Indices
    addStorageBuffer("instances", 3, sizeof(GPUInstance),
                     Constants::PathTracer::MAX_INSTANCES);
    addStorageBuffer("materials", 4, sizeof(GPUMaterial),
                     Constants::PathTracer::MAX_INSTANCES);

    addRenderTarget("MainColorOutput", 0);
    addRenderTarget("PostProcessedOutput", 1);
    addRenderTarget("Normal", 2);
    addRenderTarget("Albedo", 2);
    addRenderTarget("Emissive", 4);
    addRenderTarget("IMR", 5);
    addRenderTarget("Depth", 6);
    addRenderTarget("Hit", 7);

    addShaderPass("gbuffer", "shaders/pathTracing/main/gbuffer.comp");
    addShaderPass("renderPass", "shaders/pathTracing/main/pathTracer.comp");
    addShaderPass("post", "shaders/pathTracing/main/post.comp");
    addShaderPass("invert", "shaders/compute/invert.comp", false);

    // Display the main output by default
    setDisplayTarget("PostProcessedOutput");

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
void PathTracer::render(EngineState &state) {

    CameraState cameraState = state.scene.camera;

    // Check no texture size
    if (currentWidth == 0 || currentHeight == 0)
        return;

    if (cameraState.cameraDirty) {
        frameCount = 0;
    }

    // Increase the tracked frame count
    frameCount++;

    // Get the data flattened for the buffers
    // TODO: Rework
    flattenScene();

    // Loop each shader pass
    for (auto &pass : shaderPasses) {
        if (!pass.enabled) // If the pass isn't enabled, skip it
            continue;

        // Bind the shader & global uniforms to it
        pass.shader.bind();
        bindGlobalUniforms(pass.shader, cameraState);

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
void PathTracer::present(int width, int height) const {

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

// Register another SSBO buffer to be sent to the shader passes
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

// Add another render target for the pass tracer, passes as a Texture2D
// TODO: Figure out textures vs render targets
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

// Add a new compute shader pass to be ran
// Enabled is defaulted to true
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

// Allocate/register a binding location and space for the texture on the gpu
void PathTracer::allocateRenderTarget(RenderTarget &target) {

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

// Bind the render target's texture to the gpu
void PathTracer::bindRenderTarget(RenderTarget &target) {

    // Bind the texture to it's binding index
    glBindImageTexture(target.bindingIndex, target.id, 0, GL_FALSE, 0,
                       GL_WRITE_ONLY, target.format);
}

// -- Buffer management

// Update the buffer data; keyed by name
void PathTracer::updateBuffer(const std::string &name, const void *data,
                              size_t elementCount) {
    // Check for a non-zero update size
    if (elementCount == 0)
        return;

    // Find the keyed buffer
    const auto ittr = storageBuffers.find(name);
    if (ittr == storageBuffers.end()) {
        // If the buffer doesn't exist, send an error and return out
        Logger::error("RENDERER",
                      "PathTracer::updateBuffer - unknown buffer: " + name);
        return;
    }

    // Fetch the PersistentBuffer
    PersistentBuffer &buffer = ittr->second;

    // Send the update command, with calculating the size
    buffer.update(data, elementCount * buffer.elementSize);
}

// --- Scene managemnet ---

// Flatten the active scene data for use
void PathTracer::flattenScene() {

    Scene activeScene = engineContext.getScene()->scene;

    // Grab all of the renderables from the scene
    const auto renderables =
        activeScene.getMatchingEntities<TransformComponent, MeshComponent,
                                        MaterialComponent>();

    // Loop each entity id
    for (EntityID id : renderables) {

        // Get the mesh component and check to see if it changed brender section
        // TODO: Here? maybe check for dirty flags in the
        auto &meshComp = activeScene.getComponent<MeshComponent>(id);
        if (instanceLookupTable.find(meshComp.assetID) ==
            instanceLookupTable.end()) {
            rebuildGeometryLookupTable(activeScene);
            break;
        }
    }

    // Clear the instances cache
    instances.clear();
    instances.reserve(renderables.size());

    // TODO: Material dirty flag
    materialList.clear();
    materialList.reserve(renderables.size());

    for (EntityID id : renderables) {

        // Get each component for rendering
        auto &meshComp = activeScene.getComponent<MeshComponent>(id);
        auto &transform = activeScene.getComponent<TransformComponent>(id);
        auto &materialComp = activeScene.getComponent<MaterialComponent>(id);

        // Create a new instance
        GPUInstance inst{};

        // Claculate the model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
        model *= glm::mat4_cast(transform.rotation);
        model = glm::scale(model, transform.scale);

        // Set the instance values
        inst.transform = model;
        inst.invTransform = glm::inverse(model);
        inst.meshIndex = instanceLookupTable[meshComp.assetID];
        // TODO: materialID

        // Push back the new instance
        instances.push_back(inst);

        // Create a new GPUMaterial and set the values from the component
        GPUMaterial mat{};
        mat.albedo = glm::vec4(materialComp.albedo, 0.0f);
        mat.emmissive = glm::vec4(materialComp.emmissive, 0.0f);
        mat.roughness = materialComp.roughness;
        mat.metallic = materialComp.metallic;

        // Push back the new material
        materialList.push_back(mat);
    }

    // Update the buffers w/ the data
    updateBuffer("instances", instances.data(), instances.size());
    updateBuffer("materials", materialList.data(), materialList.size());
}

// Rebuild the lookup table for geometry; used when geometry is dirty
void PathTracer::rebuildGeometryLookupTable(Scene &activeScene) {

    // Save the new vectors for the lookup table
    std::vector<GPUVertex> instanceVertices;
    std::vector<uint32_t> instanceIndices;
    std::vector<GPUMeshEntry> meshEntries;

    // Clear the old data
    instanceLookupTable.clear();

    // Get all of the mesh componentes and loop each id
    auto renderables = activeScene.getMatchingEntities<MeshComponent>();

    // Loop each asset id in the unique meshes
    for (const auto &entityID : renderables) {

        // Load the mesh data
        auto meshData =
            activeScene.getComponent<MeshComponent>(entityID).meshData;

        // Create a new mesh entry and set the data
        GPUMeshEntry entry{};
        entry.baseVertex = static_cast<uint32_t>(instanceVertices.size());
        entry.baseIndex = static_cast<uint32_t>(instanceIndices.size());
        entry.indexCount = static_cast<uint32_t>(meshData.indices.size());

        // Loop each vertex
        for (const auto &v : meshData.vertices) {

            // Push the vertecies to the instance verts
            instanceVertices.push_back({glm::vec4(v.position, 1.0f),
                                        glm::vec4(v.normal, 0.0f),
                                        glm::vec4(v.texCoords, 0.0f, 0.0f)});
        }

        // Insert the instanceIndicies
        instanceIndices.insert(instanceIndices.end(), meshData.indices.begin(),
                               meshData.indices.end());

        // Set the instance lookup table, indexed by the asset id
        instanceLookupTable[meshData.name] =
            static_cast<uint32_t>(meshEntries.size());

        // Push the new mesh entries
        meshEntries.push_back(entry);
    }

    // Update the buffer data
    updateBuffer("meshEntries", meshEntries.data(), meshEntries.size());
    updateBuffer("vertices", instanceVertices.data(), instanceVertices.size());
    updateBuffer("indices", instanceIndices.data(), instanceIndices.size());
}

// --- Uniforms ---

// Bind the global uniforms that every shader pass has
// TODO: Move to a UBO / reimplement it
void PathTracer::bindGlobalUniforms(Shader &shader,
                                    const Camera &camera) const {
    // TODO: UBO for at least defaults & dynamic uniforms
    shader.setUInt("uFrameNum", frameCount);
    shader.setVec3("uCameraPos", camera.position);
    shader.setFloat("uFOV", camera.fov);
    shader.setInt("uInstanceCount", static_cast<int>(instances.size()));
    shader.setMat4("uInverseView", glm::inverse(camera.getViewMatrix()));
}

// --- Shader pass management ---

// Run the shader pass, dispatching the compute shaders
void PathTracer::dispatchShaderPass(const ShaderPass &pass) const {

    // Calculate the group sizes
    const GLuint groupsX =
        (currentWidth + pass.workgroupSize.x - 1) / pass.workgroupSize.x;
    const GLuint groupsY =
        (currentHeight + pass.workgroupSize.y - 1) / pass.workgroupSize.y;
    // TODO: groupsZ

    // Dispatch the shader
    glDispatchCompute(groupsX, groupsY, pass.workgroupSize.z);

    // Set the memory barriers for the image, storage, and fbo barrier bits
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
                    GL_SHADER_STORAGE_BARRIER_BIT | GL_FRAMEBUFFER_BARRIER_BIT);
}

} // namespace Engine
