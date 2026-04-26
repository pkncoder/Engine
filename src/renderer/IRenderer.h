#pragma once

class IRenderer {
  public:
    virtual ~IRenderer() = default; // Deconstructor

    virtual void Init() = 0; // Initializing a renderer

    virtual void Render(const class Camera &camera,
                        float aspectRatio) = 0; // Rendering a frame

    virtual void Shutdown() = 0; // Shutting down a renderer
};
