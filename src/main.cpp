#include "core/Application.h"
#include "services/Logger.h"

#ifdef __APPLE__
#include <filesystem>
#include <iostream>
#include <mach-o/dyld.h>
#endif

void setWorkingDirectoryToBinary() {
#ifdef __APPLE__
#if RUN_PATH_FIX
    char path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        std::filesystem::path exePath(path);
        // Change working directory to the directory containing the
        // executable
        std::filesystem::current_path(exePath.parent_path());
    }

#else
    std::cout << "[Dev Mode] Skipping macOS path override. Working directory "
                 "left as default.\n";
#endif
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
