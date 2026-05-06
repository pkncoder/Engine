#include "PathTracer.h"
#include "../resources/AssetManager.h"
#include "../scene/components/MeshComponent.h"
#include "../scene/components/TransformComponent.h"
#include "../services/Logger.h"

#include <set>

namespace Engine {

void PathTracer::init() {
    computeShader = Shader("shaders/compute/path_tracer.comp");

    if (glDispatchCompute == nullptr) {
        Logger::fatal("RENDERER",
                      "Compute Shaders are not supported on this system!");
        return;
    }

    glGenFramebuffers(1, &presentFBO);

    // Allocate the Dynamic Instance Buffer up front.
    // We allocate enough space for MAX_INSTANCES.
    instanceBuffer.setup(GL_SHADER_STORAGE_BUFFER,
                         MAX_INSTANCES * sizeof(GPUInstance));

    // Bind the buffers to their respective GLSL binding points
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, instanceBuffer.id);
}

void PathTracer::resize(int newWidth, int newHeight) {
    if (newWidth == currentWidth && newHeight == currentHeight)
        return;

    currentWidth = newWidth;
    currentHeight = newHeight;

    if (outputTexture != 0)
        glDeleteTextures(1, &outputTexture);

    glGenTextures(1, &outputTexture);
    glBindTexture(GL_TEXTURE_2D, outputTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, currentWidth, currentHeight, 0,
                 GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindImageTexture(0, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY,
                       GL_RGBA32F);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void PathTracer::flattenScene(Scene &activeScene) {
    auto renderables =
        activeScene.getMatchingEntities<Transform, MeshComponent>();

    // 1. Check if we need to rebuild the Static Geometry Atlas
    for (EntityID id : renderables) {
        auto &meshComp = activeScene.getComponent<MeshComponent>(id);
        if (meshToAtlasMap.find(meshComp.assetID) == meshToAtlasMap.end()) {
            geometryDirty = true;
            break;
        }
    }

    if (geometryDirty) {
        rebuildGeometryAtlas(activeScene);
    }

    // 2. Update Dynamic Instances
    std::vector<GPUInstance> instances;
    instances.reserve(renderables.size());

    for (EntityID id : renderables) {
        auto &transform = activeScene.getComponent<Transform>(id);
        auto &meshComp = activeScene.getComponent<MeshComponent>(id);

        GPUInstance inst{};

        // Build matrices
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
        model *= glm::mat4_cast(transform.rotation);
        model = glm::scale(model, transform.scale);

        inst.transform = model;
        inst.invTransform = glm::inverse(model);
        inst.meshIndex = meshToAtlasMap[meshComp.assetID];

        instances.push_back(inst);
    }

    // Push to GPU using persistent mapped pointer
    if (!instances.empty()) {
        instanceBuffer.update(instances.data(),
                              instances.size() * sizeof(GPUInstance));
    }
}

void PathTracer::rebuildGeometryAtlas(Scene &activeScene) {
    std::vector<GPUVertex> atlasVertices;
    std::vector<uint32_t> atlasIndices;
    std::vector<GPUMeshEntry> meshEntries;

    meshToAtlasMap.clear();

    // Get unique assets
    std::set<std::string> uniqueAssets;
    auto renderables = activeScene.getMatchingEntities<MeshComponent>();
    for (EntityID id : renderables) {
        uniqueAssets.insert(
            activeScene.getComponent<MeshComponent>(id).assetID);
    }

    // Stitch meshes together
    for (const auto &assetID : uniqueAssets) {
        // Adjust this depending on how your AssetManager returns data
        auto meshData = AssetManager::loadMesh(assetID);

        GPUMeshEntry entry{};
        entry.baseVertex = static_cast<uint32_t>(atlasVertices.size());
        entry.baseIndex = static_cast<uint32_t>(atlasIndices.size());
        entry.indexCount = static_cast<uint32_t>(meshData->indices.size());

        for (const auto &v : meshData->vertices) {
            atlasVertices.push_back({glm::vec4(v.position, 1.0f),
                                     glm::vec4(v.normal, 0.0f),
                                     glm::vec4(v.texCoords, 0.0f, 0.0f)});
        }

        atlasIndices.insert(atlasIndices.end(), meshData->indices.begin(),
                            meshData->indices.end());

        meshToAtlasMap[assetID] = static_cast<uint32_t>(meshEntries.size());
        meshEntries.push_back(entry);
    }

    // Allocate and map static buffers
    vertexBuffer.setup(GL_SHADER_STORAGE_BUFFER,
                       atlasVertices.size() * sizeof(GPUVertex));
    vertexBuffer.update(atlasVertices.data(),
                        atlasVertices.size() * sizeof(GPUVertex));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertexBuffer.id);

    indexBuffer.setup(GL_SHADER_STORAGE_BUFFER,
                      atlasIndices.size() * sizeof(uint32_t));
    indexBuffer.update(atlasIndices.data(),
                       atlasIndices.size() * sizeof(uint32_t));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, indexBuffer.id);

    meshEntryBuffer.setup(GL_SHADER_STORAGE_BUFFER,
                          meshEntries.size() * sizeof(GPUMeshEntry));
    meshEntryBuffer.update(meshEntries.data(),
                           meshEntries.size() * sizeof(GPUMeshEntry));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, meshEntryBuffer.id);

    geometryDirty = false;
}

void PathTracer::render(const Camera &camera, Scene &activeScene,
                        float aspectRatio) {
    if (currentWidth == 0 || currentHeight == 0)
        return;

    // 1. Sync Data
    flattenScene(activeScene);

    // 2. Bind
    computeShader.bind();
    glBindImageTexture(0, outputTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY,
                       GL_RGBA32F);

    // 3. Set basic Uniforms
    computeShader.setVec3("u_CameraPos", camera.position);
    // You will need to pass inverse view/proj to cast rays:
    // computeShader.setMat4("u_InverseView",
    // glm::inverse(camera.getViewMatrix()));
    // computeShader.setMat4("u_InverseProj",
    // glm::inverse(camera.getProjectionMatrix()));

    // 4. Dispatch (8x8 blocks)
    GLuint numGroupsX = (currentWidth + 7) / 8;
    GLuint numGroupsY = (currentHeight + 7) / 8;
    glDispatchCompute(numGroupsX, numGroupsY, 1);

    // 5. Memory Barrier
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void PathTracer::shutdown() {
    vertexBuffer.shutdown();
    indexBuffer.shutdown();
    meshEntryBuffer.shutdown();
    instanceBuffer.shutdown();

    if (outputTexture != 0) {
        glDeleteTextures(1, &outputTexture);
        outputTexture = 0;
    }
}

void PathTracer::presentTextureToFramebuffer(int width, int height) {
    // Bind our temporary FBO for reading
    glBindFramebuffer(GL_READ_FRAMEBUFFER, presentFBO);

    // Attach the Path Tracer's output texture to it
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, getOutputTexture(), 0);

    // Bind the default window buffer for drawing
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // Blit (copy) the pixels from the texture FBO to the screen
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

} // namespace Engine
