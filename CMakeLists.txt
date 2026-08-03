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
find_package(tinyobjloader CONFIG REQUIRED)

find_package(Stb REQUIRED)
# Create the missing modern target if it doesn't exist
if(NOT TARGET stb::stb)
    add_library(stb::stb INTERFACE IMPORTED)
    target_include_directories(stb::stb INTERFACE ${Stb_INCLUDE_DIRS})
endif()

# --- Source Files ---
# Note: glad.c is now handled by the library, no need to include it manually!
set(SOURCES
    src/main.cpp
    src/core/layers/LayerStack.cpp
    src/core/layers/RendererLayer.cpp
    src/core/layers/SceneUpdateLayer.cpp
    src/core/Application.cpp
    src/core/EngineContext.cpp
    src/core/Window.cpp
    # src/renderer/BufferManager.cpp
    # src/renderer/PathTracer.cpp
    # src/renderer/Rasterizer.cpp
    # src/renderer/RendererManager.cpp
    # src/renderer/Shader.cpp
    src/resources/AssetManager.cpp
    src/resources/MaterialLoader.cpp
    src/resources/ModelLoader.cpp
    src/resources/TextureLoader.cpp
    src/scene/Camera.cpp
    src/scene/PrefabSpawner.cpp
    src/scene/Scene.cpp
    src/scene/SceneManager.cpp
    src/services/Input.cpp
    src/services/Logger.cpp
    src/services/Timer.cpp
    src/services/UUID.cpp
)

# --- Executable ---
add_executable(${PROJECT_NAME} ${SOURCES})

# --- Linking ---
# We link against the "Namespaced" targets provided by the packages
target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw 
    glad::glad 
    glm::glm
    tinyobjloader::tinyobjloader
    stb::stb
)
