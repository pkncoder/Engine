#include "core/Application.h"
#include "services/Logger.h"

#ifdef __APPLE__
#include <filesystem>
#include <iostream>
#include <mach-o/dyld.h>
#endif

void setWorkingDirectoryToBinary() {
#if RUN_PATH_FIX
#ifdef __APPLE__
    char path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        std::filesystem::path exePath(path);
        // Path is: Engine.app/Contents/MacOS/engine
        // Moving up 4 times escapes the bundle and lands in the zip extraction
        // directory
        std::filesystem::path rootDir = exePath
                                            .parent_path()  // MacOS/
                                            .parent_path()  // Contents/
                                            .parent_path()  // Engine.app/
                                            .parent_path(); // Zip Root/

        std::filesystem::current_path(rootDir);
    }
#endif
#else
    std::cout << "[Dev Mode] Keeping default path context.\n";
#endif
    // Windows and Linux usually handle this natively,
    // but std::filesystem::current_path() can verify or set it there too.
    std::cout << "Current Working Directory: "
              << std::filesystem::current_path() << std::endl;
}

int main() {

    setWorkingDirectoryToBinary();

    Engine::Logger::info("SYSTEM", "Startup");

    // Create the application object
    auto app = std::make_unique<Engine::Application>();

    // Init and start the app
    app->init();
    app->run();

    // Return all is good if got this far
    return 0;
}
