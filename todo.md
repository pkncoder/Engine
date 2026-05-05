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

## Todos

### Todo - Code

- [ ] Extend and use Defaults.h more
- [ ] Figure out how getting all of the "renderables" will work
  - Some maynot have materials, some may not be lights
  - Force them to have these, exclude lights?
- [ ] Check to see how acurate line numbers are in errors from shader code, try to fix if off too much / inconsistant.
- [ ] Add to timer a "periodic" way to run code
  - IDEA: Use % and a total deltaTime / time to run whenever
- [ ] Standize and fix "attributes first, then methods"", or vice-versa in header files

### Todo - Not Code

- [ ] Change how .mtl and .obj files are stored
- [ ] Figure out the issue on MAC when clicking out and struggling to get back into the window
- [ ] Should AssetTypes really be called that?
- [ ] BufferManger.cpp/h inside of renderer, or resources?
- [ ] Decide how AssetTypes.h should be layed out, and if I should split up the file.

### Todo - Future Todos

- [ ] Use EnTT instead of custom ECS
- [ ] Make live aspect ratio drawing
- [ ] Orbital camera
- [ ] Add a worldpos camera movement overload
- [ ] Custom errors
- [ ] Multithread

### Todo - Editor

- [ ] MD - Try to get checkboxes rendering
      bool isValid() const { return m_ID != NULL_ENTITY && m_Scene != nullptr; }
- [ ] MD - Get rid of the line length warning
- [ ] TMUX - Make keybinds to run launch, run, and test bash scripts
- [ ] TMUX - Change to catpuccin Moccioto or something like that
- [ ] CPP - gcc (compent) keybind not working on single lines (sometimes)
- [ ] NVIM - Make it stop fuly hiding git ignored files
- [ ] Change Leader+Arrows to Leader+Shift+Varrows

## Ideas

- [ ] Add the ability for multiple render methods together (like a path traced scene + ray marched clouds)
- [ ] Make the engine launch while loading, but show that it is "working" on it
- [ ] Optomization [Small, Tiny unless on HUGE models] -> merge normals together if they are really similar to try and reduce the total vertex count

## Steps

### Refactor Checklist

1. [ ] Fix warnings
2. [ ] Fix public vs. private
3. [ ] Make variables & functions const when needed
4. [ ] Fix placement of functions
5. [ ] Refactor step by step in code
6. [ ] Rename things
7. [ ] Add comments
8. [ ] Place the \#includes in the right spots
9. [ ] Fix file tree

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
