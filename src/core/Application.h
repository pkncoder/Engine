#pragma once

#include "../scene/Camera.h"
#include "../scene/Scene.h"
#include "Window.h"

#include <memory>

namespace Engine {

class Application {

  public:
    // Constructor & Deconstructor
    Application();
    ~Application();

    // Constructing the active render, window, etc.
    void init();

    // Main loop
    void run();

  private:
    // Presenting a texture to the screen
    void presentToScreen();

    // Handling inputs
    void handleInputs();

    // Registering components
    // TODO: temp
    void registerSceneComponents(Scene &scene);

    // Loading the models + allocating the entities
    // TODO: temp
    void setupEntities(Scene &scene);

  private:
    // Window information
    std::unique_ptr<Window> window;

    // Scene object
    Scene activeScene;

    // Camera object
    Camera camera;
};

} // namespace Engine
