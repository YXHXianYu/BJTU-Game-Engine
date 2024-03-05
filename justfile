alias gen := generate
alias g := generate
alias b := build
alias r := run

# build and run
default: build run

# generate project buildsystem to `build/`
generate:
    cmake -B build

# build the project
build: generate
    cmake --build build

# run the built binary
run:
    ./bin/BJTU-Game-Engine
