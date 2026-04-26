#pragma once

class IRenderer {
  public:
    virtual ~IRenderer() = default; // Deconstructor

    virtual void init() = 0; // Initializing a renderer

    virtual void render(const class Camera &camera,
                        float aspectRatio) = 0; // Rendering a frame

    virtual void shutdown() = 0; // Shutting down a renderer
};
