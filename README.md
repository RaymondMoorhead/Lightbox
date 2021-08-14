# Lightbox

A C++ OpenGL demo project, featuring Blinn-Phong shading and Shadow Mapping using loaded GLTF models.

The code has written and tested on Ubuntu 20.04, but with the apporpiate libraries should run on most machines. Compilation is done with g++. 

* Loading is performed by [glad](https://github.com/Dav1dde/glad) for gl 4.6
* Context creation and input are done with [GLFW](https://www.glfw.org/)
* Debug windows are created with [ImGui](https://github.com/ocornut/imgui)
* 3D rotation visualizer widget made with [imGuIZMO.quat](https://github.com/BrutPitt/imGuIZMO.quat)
* JSON file reading by [Nlohmann's JSON for Modern C++](https://github.com/nlohmann/json)
