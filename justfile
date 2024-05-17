# Set shell for Windows OSs:
set windows-shell := ["powershell.exe", "-NoLogo", "-Command"]

alias gen := generate
alias g := generate
alias b := build
alias r := run

# build and run
default: build run

# generate project buildsystem to `build/`
generate:
    python ./scripts/glsl-preprocessor/glsl-preprocessor.py
    cmake -B build

# build the project
build: generate
    cmake --build build -j8

# run the built binary
run:
    ./bin/BJTU-Game-Engine
