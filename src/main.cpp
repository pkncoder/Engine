#include "core/Application.h"

int main() {

    // Create the application object
    auto app = std::make_unique<Engine::Application>();

    // Init and start the app
    app->init();
    app->run();

    // Return all is good if got this far
    return 0;
}
