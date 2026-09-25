cmake_minimum_required(VERSION 3.15) # Manifest mode works best with 3.15+
project(engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Find Dependencies ---
# vcpkg makes these available to find_package
find_package(EnTT CONFIG REQUIRED)
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
    src/renderer/buffers/BufferManager.cpp
    src/renderer/shaders/IProgram.cpp
    src/renderer/shaders/IShader.cpp
    src/renderer/renderGraph/RenderGraph.cpp
    src/renderer/renderers/DeferredRenderer.cpp
    # src/renderer/renderers/PathTracer.cpp
    # src/renderer/renderers/Rasterizer.cpp
    src/renderer/GPUResourceManager.cpp
    src/renderer/IRenderer.cpp
    src/renderer/RendererManager.cpp
    src/resources/AssetManager.cpp
    src/resources/MaterialLoader.cpp
    src/resources/ModelLoader.cpp
    src/resources/TextureLoader.cpp
    src/scene/PrefabSpawner.cpp
    src/scene/Scene.cpp
    src/scene/SceneManager.cpp
    src/services/Input.cpp
    src/services/Logger.cpp
    src/services/Timer.cpp
    src/services/UUIDGenerator.cpp
    src/system/camera/CameraSystem.cpp
)

# --- Executable ---
if(APPLE)
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES})
else()
  add_executable(${PROJECT_NAME} ${SOURCES})
endif()


# --- Linking ---
# We link against the "Namespaced" targets provided by the packages
target_link_libraries(${PROJECT_NAME} PRIVATE
    EnTT::EnTT
    glfw
    glad::glad
    glm::glm
    tinyobjloader::tinyobjloader
    stb::stb

)

# Link Win32 system libraries when targeting Windows
if(WIN32 AND MINGW)
  target_link_options(${PROJECT_NAME} PRIVATE -static)
endif()

if(UNIX AND NOT APPLE)
  target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++)
endif()


if(APPLE)
  set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "Engine"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourname.engine"
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "@executable_path/../Frameworks"
    )

  # Automatically copy your loose asset folders next to the .app bundle post-build
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../../../shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/assets"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../../../assets"
        COMMENT "Copying loose assets and shaders next to the .app bundle"
    )

  # Sign ONLY the binary bundle (assets outside stay unsigned and modifiable)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND codesign --force --deep --sign - $<TARGET_BUNDLE_DIR:${PROJECT_NAME}>
        COMMENT "Applying local ad-hoc code signature to bundle"
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_definitions(${PROJECT_NAME} PRIVATE RUN_PATH_FIX=1)
else()
  target_compile_definitions(${PROJECT_NAME} PRIVATE RUN_PATH_FIX=0)
endif()
