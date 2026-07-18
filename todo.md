# TODO

This is a bit more than JUST todos.

This is my catalogue of todos, mission statements, and checklists across the project. This file also "stores" progress, as the todos that end up in here and stay, get saved at the bottom, going hand-in-hand with the git commit tree.

This file does not store any "// TODO : ~~~" marks I make in code, as those are meant for listing extra context in place

## TOC

<!--toc:start-->

- [TODO](#todo)
  - [Mission Statements](#mission-statements)
  - [Todos](#todos)
    - [Todo - Code](#todo---code)
    - [Todo - Not Code](#todo---not-code)
    - [Todo - Future Todos](#todo---future-todos)
    - [Todo - Editor](#todo---editor)
  - [Ideas](#ideas)
  - [Steps](#steps)
    - [Refactor Checklist](#refactor-checklist)
  - [Finished Todos](#finished-todos)
  <!--toc:end-->

## Mission Statements

## Development Phases

- [x] Phase I
  - [x] Project Setup
  - [x] First Window
- [x] Phase II
  - [x] Hello Triangle
- [x] Phase III
  - [x] Basic Scene
  - [x] Basic Asset Manager
  - [x] Basic Rasterizer & Path Tracer
    - [x] Renderer
    - [x] Shaders
- [ ] Phase IV
  - [ ] Scene Manager
  - [ ] Advanced Renderers & Shaders
    - [ ] Rasterizer
    - [ ] Ray Tracer
    - [ ] Path Tracer
    - [ ] Ray Marcher
  - [ ] Acceleration Structures
  - [ ] Scene & Render Exporting
- [ ] Phase V
  - [ ] UI/UX
- [ ] Phase VI
  - Rewire

## Todos

### Todo - Code

- [ ] Figure out how getting all of the "renderables" will work with different object types
- [ ] Make the outputTexture start with everything at vec3(1.0, 0.0, 1.0); for debugging
- [ ] Figure out the issue w/ width & height passing being a lot
- [ ] Use a UBO for passing uniforms
- [ ] Material Index in GPUInstance
- [ ] Fix the flashing compute shader issue + maybe being too slow?
- [ ] Non-triangle objects
- [ ] Missing texture for failed loads default
- [ ] Go through and remove GPU memory access where it shouldn't be
- [ ] Check to see if roughness & metallic maps are working
- [ ] Get the shared uniforms file filled in and finished
- [ ] UUID models & mesh names
- [ ] Redo the shader pass & render target sytem to increase scope
- [ ] Texture data
- [ ] Create multiple I... files for compute shaders, textures, etc.
- [ ] Figure out where to place the window height/width
- [ ] Figure out constant rules
- [ ] Figure out inline class attribute rules
- [ ] Move window into EngineContext?
- [ ] Linux randomly dips in performance
- [ ] Go around making shutdown functions & deconstructors (they just call shutdown)
- [ ] Figure out moving around engineState so everything has the same object
- [ ] Figure out how to handle setting glfw & glad settings; if it should just be a function like it is now, or different
- [ ] Application is dumping core on quit

### Todo - Not Code

- [ ] Finish the readme
- [ ] Update P_III_Archetecture to the style of P_IV_Planned_Archetecture

### Todo - Future Todos

- [ ] Use EnTT instead of custom ECS
- [ ] Make live aspect ratio drawing
- [ ] Different camera movement modes
- [ ] Custom errors
- [ ] Multithread
- [ ] Create debug textures that can be viewed from the viewport
- [ ] Multi-pass setup for renderers
- [ ] Create & Finish multiple renderers
  - [ ] Rasterizer
  - [ ] Real-time Ray Tracer
  - [ ] Real-time Path Tracer
  - [ ] Real-time Ray Marcher
  - [ ] Real-time Ray Caster (Voxels)
- [ ] Different shader models
  - Ex. Disney BSDF
- [ ] Legacy OpenGL support
- [ ] Move camera to ECS w/ CameraComponent, and move the camera class to the system directory

### Todo - Editor

- [ ] MD - Try to get checkboxes rendering
      bool isValid() const { return m_ID != NULL_ENTITY && m_Scene != nullptr; }
- [ ] MD - Get rid of the line length warning
- [ ] TMUX - Make keybinds to run launch, run, and test bash scripts
- [ ] TMUX - Change to catpuccin Moccioto or something like that
- [ ] CPP - gcc (compent) keybind not working on single lines (sometimes)
- [ ] NVIM - Make it stop fuly hiding git ignored files
- [ ] Change Leader+Arrows to Leader+Shift+(V)arrows

## Ideas

- [ ] Add the ability for multiple render methods together (like a path traced scene + ray marched clouds)
- [ ] Make the engine launch while loading, but show that it is "working" on it
- [ ] Optomization [Small, Tiny unless on HUGE models] -> merge normals together if they are really similar to try and reduce the total vertex count
- [ ] Non-Euclideon geometry
- [ ] Marching Spheres
- [ ] Scene state manager. A class to track the "dirty" flags instead of implementing it into every renderer
- [ ] Allow for NeRFs (Neural Radience Feilds)
- [ ] "Version Bitfield Signatures" for very quick updates
  - Ex. RendererSettings has the active render updated, so it changes it's bit on the signature. Instead of the onEvent going through each change to see if a version matches, do a bitwise or (or something) to find the exact updated thing
- [ ] Multiple windows
- [ ] Vulkan option

## Steps

### Refactor Checklist

1. [x] Fix warnings
2. [x] Fix public vs. private
3. [x] Make functions const when needed
4. [x] Make function params const when needed
5. [x] Make functions inline when needed
6. [x] Fix placement of functions
7. [ ] Refactor step by step in code
8. [ ] Rename things
9. [ ] Add comments
10. [ ] Place the \#includes in the right spots
11. [ ] Fix file tree
12. [ ] Run ```./fullTest```
13. [ ] Update the obsidian canvas & png
14. [ ] Update the website

## Finished Todos

- [x] Figure out how individual pannels get their information.
  - It doesn't make much sense for individual panels just connect to different points of information, this is an overall issue, however needs to be addressed.
- [x] Fix indent size
- [x] Fix naming conventions
- [x] Fix aspect ratio calculation
- [x] Fix camera.h name + shader.cpp/h
- [x] Fix refactor-checklist
- [x] Add a "PreRender" function to Window
- [x] Move inputs into seperate thing
- [x] Extend the timer class
- [x] Fix the Timer::Log discrepancy
  - Currently, logging means run it every frame and maybe log
- [x] Re-fit file tree to figure out what files are just header files, and what files have header AND c++ files.
- [x] Make sure everything that should be in the engine namespace, is
- [x] Make github repo
- [x] Check for not foo_bar, and repladce with fooBar
- [x] Figure out how shader including will work in current code
- [x] Create a full Logger class that can then be used later in UI
- [x] Move to logger instead of cout
- [x] Create a setting in Logger that will output a log every time you send one instead of send it to pending
- [x] Integrate timer's profiler with the logger
- [x] Standize and fix "attributes first, then methods"", or vice-versa in header files
- [x] Check to see how acurate line numbers are in errors from shader code, try to fix if off too much / inconsistant.
- [x] Extend the tags in Logger calls
- [x] BufferManger.cpp/h inside of renderer, or resources?
- [x] Figure out the issue on MAC when clicking out and struggling to get back into the window
- [x] Add to timer a "periodic" way to run code
  - IDEA: Use % and a total deltaTime / time to run whenever
- [x] Move all these dang structs
- [x] Logger DEBUG LogLevel
- [x] Should AssetTypes really be called that?
- [x] Decide how AssetTypes.h should be layed out, and if I should split up the file.
- [x] Make a method to spawn entities easeir than it is
- [x] Change TransformComponent's struct declaration from Transform to TransformComponent
- [x] Unabstract "model" to "obj" to prepare for way in the future, other model files
- [x] "emmision" -> "emmisive"
- [x] Fix the issue on the path tracer on overloads
- [x] Change how .mtl and .obj files are stored
- [x] Fix average FPS
- [x] Make all basic inits inlines
- [x] Get rid of all of the "magic numbers" in the renderers
- [x] Redo all of pathTracer.cpp for struct passing (like idk, materials)
- [x] Extend and use Defaults.h more
- [x] Multi-materials
- [x] Sub-object entities
- [x] Redo the temp rasterizer & path tracer shaders (pt 1.)
- [x] RendererManager, to manage the renderers
- [x] Rework rasterizer (pt. 1)
- [x] RenderManager or RendererManager? (yes this is very important)
- [x] Fix the periodic timer to work with: one-second intervals, fractional intervals, and make sure that it isn't skipping intervals (or clear it)
- [x] Try to reduce the amount of looping being done on materials / instances etc. Maybe try to use the caches in AssetManager more?
- [x] Plan out the texture handle nonsense
- [x] Add a SceneManager
- [x] Make dirty mesh/camera flags (REWORKED)
- [x] Move resize callback from Input -> Window
