cmake_minimum_required(VERSION 3.15) # Manifest mode works best with 3.15+
project(engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Find Dependencies ---
# vcpkg makes these available to find_package
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# --- Source Files ---
# Note: glad.c is now handled by the library, no need to include it manually!
set(SOURCES
    src/main.cpp
    src/core/Application.cpp
    src/core/Window.cpp
    src/core/Input.cpp
    src/services/Timer.cpp
    src/scene/Camera.cpp          # <--- Check these!
    src/renderer/Rasterizer.cpp
    src/renderer/BufferManager.cpp # <--- Check these!
    src/renderer/Shader.cpp        # <--- Check these!
)

# --- Executable ---
add_executable(${PROJECT_NAME} ${SOURCES})

# --- Linking ---
# We link against the "Namespaced" targets provided by the packages
target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw 
    glad::glad 
    glm::glm
)
