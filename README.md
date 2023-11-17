This project takes as input a perlin noise texture and uses it as a heightmap for some simple terrain.

```
./perlin-terrain <path to perlin noise texture>
perlin-terrain.exe <path to perlin noise texture> 
```

![example screenshot](docs/example.png)

This repository in **in progress**, new features will be added.

### Dependencies

The project uses:

* OpenGL: to render triangles to the screen

* GLFW: to open a window

* GLAD

* GLEW

* ImGui: to add a GUI with useful widgets

GLFW and GLEW are submodules that need to be downloaded:

```
git clone https://github.com/jlartois/perlin-terrain.git
git submodule update --init --recursive
```

They can then be found in the `lib` folder, and will be build together with the rest of the project thanks to the CMakeLists.txt.

GLAD and ImGUI are already included as `.h` and `.cpp` files in the `include` directory.

### Building

**Windows**: use CMake. OpenGL comes by default on Windows.

**Linux and MacOS**: comming soon
