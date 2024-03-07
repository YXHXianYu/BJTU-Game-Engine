# Boundless Jovial Thriving Utopia Game Engine

> BJTU Game Engine, for short

## How to build & run

1. Clone the project and enter the directory

    ```
    git clone https://github.com/YXHXianYu/BJTU-Game-Engine.git
    cd BJTU-Game-Engine
    ```

2. Run the following command to build

    ```
    python ./scripts/glsl-preprocessor/glsl-preprocessor.py
    cmake . -B build
    cmake --build build
    ```
    
    The binary `BJTU-Game-Engine` should be generated into the `bin` folder.
    
3. Run the binary.

Or, if you have [just](https://github.com/casey/just) installed, step 2~3 can be simply done by running:

```
just
```

> Check `justfile` or use `just -l` for more information.
