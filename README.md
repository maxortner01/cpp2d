# CPP2D
*The 2D spiritual successor to [**SGAL**](https://www.github.com/maxortner01/SGAL).*

This is a lightweight 2D graphics framework written in C++. It (will be) entirely self-contained, (for the most part). The intention is for only a OpenGL loader and stb to be dependencies.

Currently, GLFW is required for this to work, but I'll be removing this requirement eventually.

I will also eventually implement Vulkan, but for now OpenGL will do.

## What doesn't work
Currently, in order to build, I need to add a make command to the `src/CMakeLists.txt` that runs for `extern/glew/auto` which retreives the OpenGL extension files. Other than that, the compile should run on MacOS perfectly.

## Changelog
- Just added instancing