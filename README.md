# Boundless Jovial Thriving Utopia Game Engine

> BJTU Game Engine, for short

## Prerequisites

* C++ Compiler
  * GCC >= 13
  * MSVC (Visual Studio >= 2022)
* CMake >= 3.19
* Git >= 2.1
* Python >= 3.9

## How to Build & Run

1. Clone the project and enter the directory

    ```
    git clone https://github.com/YXHXianYu/BJTU-Game-Engine.git
    cd BJTU-Game-Engine
    ```

2. Run the following command to build

    ```
    python ./scripts/glsl-preprocessor/glsl-preprocessor.py
    cmake . -B build
    cmake --build build -j16
    ```
    
    The binary `BJTU-Game-Engine` should be generated into the `bin` folder.
    
3. Run the binary.

### Just

Or, if you have [just](https://github.com/casey/just) installed, step 2~3 can be simply done by running:

```
just
```

> Check `justfile` or use `just -l` for more information.

## How to Control

* Movement
  * Use `WASD` for moving forward, backward, left, and right, respectively.
  * Use `QE` for moving up and down.
* Camera
  * Hold the right mouse button and drag to adjust the camera view.
* Window
  * Press `Escape` and `Enter` to exit the BJTU-Game-Engine
* Render
   *  Press `Shift + K` to control whether to render the character
   *  Press `Shift + J` to control whether to render the block
   *  Press `Shift + L` to control whether to render the light mesh
   *  Press `Shift + O` to control whether to use the orthogonal projection
   *  Press `Shift + P` to control whether to render scene by depth (z-buffer)
