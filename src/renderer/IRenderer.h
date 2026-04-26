#pragma once

class IRenderer {
  public:
    virtual ~IRenderer() = default;

    virtual void Init() = 0;

    // We pass a reference to the camera or a FrameContext here
    virtual void Render(const class Camera &camera, float aspectRatio) = 0;

    virtual void Shutdown() = 0;
};
