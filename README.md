<h1 align="center">⛭  Tempname: Engine ⛭</h1>
<p align="center">A Crossplatform - GPU Multi-Level Rendering Tool</p>
<!-- <p align="center"><img width="60%" src="/Assets/AppInterface.png"></img></p> -->

## ☀️ Overview

Tempname: Engine is an application that has the plan for something not seen very often. On a high level, Tempname: Engine is a GPU-Renderer that will have model, scene, and material modifications. This, however, is not the main point.

Tempname: Engine will use the built scene and render it out in a varriety of methods. Instead of having the basic Rasterizer or minimal Render Graph, Tempname: Engine will include multiple-different renderers. Some of these include:
- Rasterizer
- Deferred Rendering
- Ray Tracer
- Path Tracer
- Ray Marcher
- Ray Caster (Voxels)

Along with these, the user will have the utilities to render out stylized scenes to their modifications, and have the ability for simple animations & scene creation.

## ✨ Features

### Current

- Renderers
  - Deferred Shader w/ Basic Render Graph
    - Shadows
    - G-Buffer
    - Lighting
- Moveable camera
- Entity Component System (ECS) using EnTT
  - MeshComponent
  - TransformComponent
  - MaterialComponent
  - CameraComponent
  - PointLightCompoment
- Object Materials
  - Albedo/Emmissive
  - Roughness/Metallic
- .obj, .mat, and texture file loading
- Terminal logger w/ scrolling information & an updating "dashboard"
  - Log level/type + Tags
  - Scrolling/stacked logging & In-place or dynamic text
  - ASCI colors

### Planned /  To-Be-Made

- Renderers
  - Ray Tracer \*
  - Ray Marcher \*
  - Ray Caster (Voxels) \*
- Larger material support
- Full editor for scene and render modifications
- Multi-render pass support \*
- Multi-renderer image outputs (ex. rasterized scene, path traced reflections/shadows, ray marched clouds) \*
- Multi-threading

> \* OpenGL 4.6+ (not MacOS)

For more todos, ideas, and current capibilites, check out: [todo.md](todo.md).

## 🚀 Running

This project has been tested on:

- M2 Macbook Air (2022), running MacOS Sequoia 15.2
- Ryzen AI 9 HX 370 Framework 16 + NVIDIA GeForce RTX 5070 8G - On Fedora Linux 43/44 + Niri WM

Due to Apple's discontinuation OpenGL, some features are not supported on the OS. Plans for compatibility shaders are wrote down, however not being worked on.


## Option 1 (Recommended)

Go to the "Releases" page on this repo, and download the right one for your system.

Any problems, please report them to the issues page.

## Option 2 - Compile Yourself

### Step 1

Download & install cmake.

It is recommend using a package manager for Unix systems (ex. brew for MacOS or dnf for Fedora) or finding a YT video if you are unable/don't want to use a package manager, but here is the cmake's download page if you want to do it the hard way: [Cmake Downloads](https://cmake.org/download/).

### Step 2

Download & install vcpkg.

Copy down the repo & cd into it:

```
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
```

Run the install script based on your system:

- Windows:

```
.\bootstrap-vcpkg.bat
```

- MacOS/Linux

```
./bootstrap-vcpkg.sh
```

### Step 3

Clone down this repo in the terminal and cd into it:

```
git clone https://github.com/pkncoder/Light-Teachings.git && cd ./Engine/
```

### Step 4

If you are using bash/zsh/similar syntax terminal langauges, you can run the "run" script like this:

```
./run
```

If that does not work, you can compile it yourself like this:

```
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$HOME/.vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
```

And then run it:

```
./build/bin/engine
```

## ⚙️ Usage

To control the scene, WASD can be used for movement, and holding right click will move the camera's looking direction.

To change the rendered model:
1. Create a new folder that will hold your mesh, materials, and textures in assets/
2. Find & open activeScene.json
3. Edit "projectDirectory" to "assets/{insert your folder}"
4. Edit "objFile" to "assets/{insert your folder}/{.obj file name}.obj"
5. Save the file
6. Either re-open the application, or press "r" to reload it during runtime

## 💻 Technologies

- **IDE** - Neovim + LazyVim
- **Language** - CPP, GLSL, CMake
- **UI/UX & Window Library** - Dear ImGUI & GLFW3
- **Rendering Library** - GLAD
- **Rendering Methods** - Rasterization, Path Tracing

## 📝 Documentation

Currently, there is no live specific documentation, but there is still some files in the project:

- [fileTree.txt](fileTree.txt) - Stores a file tree of current & planned files/directories w/ explanations of their purpose.
- [todo.md](todo.md) - Stores not only current todos, but all completed todos, project goals, etc.
- [tags.md](tags.md) - Basic text file with the explanatin of what each tag does when it appears in the Logger service
- [RESOURCES.md](RESOURCES.md) - Stores the resources used in creation & any important "shout-outs" linked.

## 🌌 Gallery

<!-- TODO: fill out -->

TODO: fill out

## 🛝 Demos

The renderers included in Tempname: Engine (not including the rasterizer) were first built and made on a website called [shadertoy](https://www.shadertoy.com), which is an online OpenGL shader runner. It uses WebGL. Here are all the current online demos of those renderers:

- Ray Tracer: [Ray Traced Glass and Shiny](https://www.shadertoy.com/view/tXyXRc)
- Path Tracer: [Almost Real-Time Path Tracer](https://www.shadertoy.com/view/7fBSzR)
- Ray Marcher is in the works

*Note: Some of these may not be completed, or fully/at all implemented in Tempname: Engine yet.*

## 📚 Resources Used

<!-- TODO: ADD HIGHLIGHTS -->

For a full list of resources used, see [RESOURCES.md](RESOURCES.md)

## ✒️ License

This project is protected under the [MIT](LICENSE) License.
