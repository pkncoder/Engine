#include "PathTracer.h"

#include "../resources/AssetManager.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Logger.h"

#include <set>

namespace Engine {

// Path tracer shader setup & memory setup
void PathTracer::init() {
    // Build the shader wrapper to get the final compute shader + program
    computeShader = Shader("shaders/compute/path_tracer.comp");

    // Check to see if compute shaders are supported, send a fatal if not
    if (glDispatchCompute == nullptr) {
        Logger::fatal("RENDERER",
                      "Compute Shaders are not supported on this system!");
        return;
    }

    // Create our framebuffer that we will blit onto
    glGenFramebuffers(1, &presentFBO);

    // Pre-alocate and setup the GPUInstance buffer with the max instances
    instanceBuffer.setup(GL_SHADER_STORAGE_BUFFER,
                         MAX_INSTANCES * sizeof(GPUInstance));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, instanceBuffer.id);
}

// Cleaning up opengl memory
void PathTracer::shutdown() {

    // Clean up all of the SSBOs' memory
    meshEntryBuffer.shutdown();
    vertexBuffer.shutdown();
    indexBuffer.shutdown();
    instanceBuffer.shutdown();

    // Clean up teh texture
    if (outputTexture != 0) {
        glDeleteTextures(1, &outputTexture);
        outputTexture = 0;
    }
}

// Dispatch the computer shader + all of the uniforms/buffers for it
void PathTracer::render(const Camera &camera, Scene &activeScene,
                        float aspectRatio) {
    // Check for a texture with 0 width & heihgt
    if (currentWidth == 0 || currentHeight == 0)
        return;

    // Sync the scene data
    // TODO: Every frame?
    flattenScene(activeScene);

    // Bind the compute shader program
    computeShader.bind();

    // Bind the texture for the compute shader
    glBindImageTexture(0, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY,
                       GL_RGBA32F);

    // Set uniforms for the compute shader
    computeShader.setVec3("u_CameraPos", camera.position);
    // You will need to pass inverse view/proj to cast rays:
    // computeShader.setMat4("u_InverseView",
    // glm::inverse(camera.getViewMatrix()));
    // computeShader.setMat4("u_InverseProj",
    // glm::inverse(camera.getProjectionMatrix()));

    // Dispatch the compute shader
    // TODO: Read more up on compute shader "blocks"
    GLuint numGroupsX = (currentWidth + 7) / 8;
    GLuint numGroupsY = (currentHeight + 7) / 8;
    glDispatchCompute(numGroupsX, numGroupsY, 1);

    // Memory flush bits (ImageAccess incase of any extra work on texture, and
    // framebuffer for bliting to the screen)
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
                    GL_FRAMEBUFFER_BARRIER_BIT);
}

// On window resize, rebuild the texture with the new dimentions
void PathTracer::resize(int newWidth, int newHeight) {

    // Check for an actual change in size
    // TODO: This shouldn't be done here
    if (newWidth == currentWidth && newHeight == currentHeight)
        return;

    // Update the saved widths
    currentWidth = newWidth;
    currentHeight = newHeight;

    // Delete the old texture
    if (outputTexture != 0)
        glDeleteTextures(1, &outputTexture);

    // Get the new texure
    glGenTextures(1, &outputTexture); // Generate
    glBindTexture(GL_TEXTURE_2D,
                  outputTexture); // Tell the texture its a texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, currentWidth, currentHeight, 0,
                 GL_RGBA, GL_FLOAT, NULL); // Allocate VRAM

    // Set parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR); // Scaling filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR); // Scaling filter

    // Set the texture mode
    glBindImageTexture(0, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY,
                       GL_RGBA32F);  // Write-Only mode
    glBindTexture(GL_TEXTURE_2D, 0); // Clean up the binding
}

// Blit, or move over the data from the texture to the framebuffer
void PathTracer::presentOutputTextureToFramebuffer(int width,
                                                   int height) const {
    // Bind the framebuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, presentFBO);

    // Move the texture ontop off the framebuffer
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, outputTexture, 0);

    // Draw the framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

// Flatten the scene geometry to the SSBOs
void PathTracer::flattenScene(Scene &activeScene) {

    // Get the scene entities that are set for rendering
    auto renderables =
        activeScene.getMatchingEntities<Transform, MeshComponent>();

    // Check each entity for if the geometry needs to be rebuilt or not
    for (EntityID id : renderables) {
        auto &meshComp = activeScene.getComponent<MeshComponent>(id);
        if (instanceLookupTable.find(meshComp.assetID) ==
            instanceLookupTable.end()) {
            geometryDirty = true;
            rebuildGeometryLookupTable(activeScene);
            break;
        }
    }

    // Get a new instances vector and reserve memory for the size of renderables
    std::vector<GPUInstance> instances;
    instances.reserve(renderables.size());

    // Loop each renderable entity
    for (EntityID id : renderables) {

        // Get the transform & mesh components
        auto &transform = activeScene.getComponent<Transform>(id);
        auto &meshComp = activeScene.getComponent<MeshComponent>(id);

        // Get a new GPUInstance
        GPUInstance inst{};

        // Build the model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
        model *= glm::mat4_cast(transform.rotation);
        model = glm::scale(model, transform.scale);

        // Set the transform matrix + the inverse matrix
        inst.transform = model;
        inst.invTransform = glm::inverse(model);

        // Set the mesh index of the instance lookup table
        inst.meshIndex = instanceLookupTable[meshComp.assetID];

        // Push the new instance to the full instances array
        instances.push_back(inst);
    }

    // Push to GPU using persistent mapped pointer
    if (!instances.empty()) {
        instanceBuffer.update(instances.data(),
                              instances.size() * sizeof(GPUInstance));
    }
}

// Rebuild the geometry data
void PathTracer::rebuildGeometryLookupTable(Scene &activeScene) {

    // Save new vectors for all of the new geometry / instance tables
    std::vector<GPUVertex> instanceVertices;
    std::vector<uint32_t> instanceIndices;
    std::vector<GPUMeshEntry> meshEntries;

    // Clear the loopup table
    instanceLookupTable.clear();

    // Get the unique mesh components (ids)
    std::set<std::string> uniqueMeshes;
    auto renderables = activeScene.getMatchingEntities<MeshComponent>();
    for (EntityID id : renderables) {
        uniqueMeshes.insert(
            activeScene.getComponent<MeshComponent>(id).assetID);
    }

    // Stitch meshes together into the vectors
    for (const auto &assetID : uniqueMeshes) {
        // Get the mesh assigned in AssetManager with the current assetID
        auto meshData = AssetManager::loadMesh(assetID);

        // Get a new GPUMeshEntry
        GPUMeshEntry entry{};
        entry.baseVertex = static_cast<uint32_t>(instanceVertices.size());
        entry.baseIndex = static_cast<uint32_t>(instanceIndices.size());
        entry.indexCount = static_cast<uint32_t>(meshData->indices.size());

        // Push all of the new verticies into instanceVertices
        for (const auto &v : meshData->vertices) {
            instanceVertices.push_back({glm::vec4(v.position, 1.0f),
                                        glm::vec4(v.normal, 0.0f),
                                        glm::vec4(v.texCoords, 0.0f, 0.0f)});
        }

        // Insert the indicies
        instanceIndices.insert(instanceIndices.end(), meshData->indices.begin(),
                               meshData->indices.end());

        // Set the lookup table with all of the mesh entries here
        instanceLookupTable[assetID] =
            static_cast<uint32_t>(meshEntries.size());

        // Push the new entry into the meshEntries vector
        meshEntries.push_back(entry);
    }

    // Vertex buffer setup, updating, and binding
    vertexBuffer.setup(GL_SHADER_STORAGE_BUFFER,
                       instanceVertices.size() * sizeof(GPUVertex));
    vertexBuffer.update(instanceVertices.data(),
                        instanceVertices.size() * sizeof(GPUVertex));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertexBuffer.id);

    // Index buffer setup, updating, and binding
    indexBuffer.setup(GL_SHADER_STORAGE_BUFFER,
                      instanceIndices.size() * sizeof(uint32_t));
    indexBuffer.update(instanceIndices.data(),
                       instanceIndices.size() * sizeof(uint32_t));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, indexBuffer.id);

    // Mesh buffer setup, updating, and binding
    meshEntryBuffer.setup(GL_SHADER_STORAGE_BUFFER,
                          meshEntries.size() * sizeof(GPUMeshEntry));
    meshEntryBuffer.update(meshEntries.data(),
                           meshEntries.size() * sizeof(GPUMeshEntry));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, meshEntryBuffer.id);

    // Un-dirty the geometry
    geometryDirty = false;
}

} // namespace Engine
