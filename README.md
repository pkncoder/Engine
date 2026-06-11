<h1 align="center">⛭  Tempname: Engine ⛭</h1>
<p align="center">A Crossplatform - GPU Multi-Level Rendering Tool</p>
<!-- <p align="center"><img width="60%" src="/Assets/AppInterface.png"></img></p> -->

## ☀️ Overview

Tempname: Engine is an application that has the plan for something not seen very often. On a high level, Tempname: Engine is a GPU-Renderer that will have model, scene, and material modifications. This, however, is not the main point.

Tempname: Engine will use the built scene and render it out in a varriety of methods. Instead of forcing the user into a basic Rasterizer and a few different qualities of a Path Tracer, the user will have control of a suite of different rendering techniques. There are some more ideas for others, but the basic renderers will include:

- Rasterizer
- Ray Tracer
- Path Tracer
- Ray Marcher
- Ray Caster (Voxels)

Along with these, the user will have the utilities to render out stylized scenes to their modifications, and have the ability for simple animations & scene creation.

## ✨ Features

### Current

- Renderers
  - Rasterizer
  - Path Tracer \*
- Moveable camera
- Basic Entity Component System (ECS)
  - MeshComponent
  - TransformComponent
  - MaterialComponent
- Object Materials
  - Albedo/Emmissive
  - Roughness/Metallic
- .obj and .mat file loading
- Terminal logger w/ scrolling information & an updating "dashboard"
  - Log level/type + Tags
  - Scrolling/stacked logging & In-place or dynamic text
  - ASCI colors

### Planned /  To-Be-Made

- Renderers
  - Ray Tracer
  - Ray Marcher
  - Ray Caster (Voxels)
  - Marching Cubes?
- Larger material support
- Full editor for scene and render modifications
- Multi-render pass support
- Multi-renderer image outputs (ex. rasterized scene, path traced reflections/shadows, ray marched clouds)
- Multi-threading

\* OpenGL 4.6+ (not MacOS)

For more todos, ideas, and current capibilites, check out: [todo.md](todo.md).

## 🚀 Running

This project has been tested on:

- M2 Macbook Air (2022), running MacOS Sequoia 15.2
- Ryzen AI 9 HX 370 Framework 16 + NVIDIA GeForce RTX 5070 8G - On Fedora Linux 43/44 + Niri WM

Due to Apple's discontinuation OpenGL, some features are not supported on the OS. Plans for compatibility shaders are wrote down, however not being worked on.

<!-- TODO: Add the dependencies -->

### Step 1

Clone down this repo in the terminal and cd into it:

```
git clone https://github.com/pkncoder/Light-Teachings.git && cd ./Engine/
```

### Step 2

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

<!-- TODO: fill out -->

TODO: fill out

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

*Note: Some of these may not be completed, or fully/at all implemented in Tempname: Engine yet.*

## 📚 Resources Used

<!-- TODO: ADD HIGHLIGHTS -->

For a full list of resources used, see [RESOURCES.md](RESOURCES.md)

## ✒️ License

This project is protected under the [MIT](LICENSE) License.
