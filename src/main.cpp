#include "core/Application.h"

int main() {

    // Create the application object
    auto app = std::make_unique<Engine::Application>();

    // Init and start the app
    app->Init();
    app->Run();

    return 0;
}
