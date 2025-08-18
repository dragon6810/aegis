# Aegis

## Project Description

Aegis is a Quake II/Half-life era game engine. It uses some techniques too advanced for the time (e.g. navigation meshes), so don't expect it to run at an acceptable level on period hardware. Along with the core engine, Aegis supplies tools to create assets for the engine.

## Project Structure

Aegis follow a modular structure, where each folder apart from `.vscode/`, `assets/`, and `run/` contains c++ subprojects. The outlier here is the `tools/` directory, which is a relic of an older structure. `tools/` contains a set of C projects with their own build system, and will soon be rewritten.

## Building

Aegis uses CMake as its primary build system (again the exception here is `tools/`, which uses Make). Currently, only MacOS is supported, but Linux support should work in the near future. 

First, you will need to install some dependencies.

One such depency is the Vulkan SDK. It can be found here:

https://vulkan.lunarg.com/sdk/home

The rest can be installed with the following commands:
    
### MacOS (brew):

    brew install glew glfw

### Ubuntu (apt):

    sudo apt install libgl1-mesa-dev libglew-dev libglfw3-dev

In order to build the project, run:

    git submodule update --init --recursive
    ./configure.sh
    ./build.sh

This will place the compiled binaries, including tests, into `build/bin`. For a streamlined way to build and run a given binary, `engine.sh`, `tests.sh`, and `wp3d.sh` will do just that. Note those scripts build every binary but only run their corresponding executable, so build time will not be any different from `build.sh`.