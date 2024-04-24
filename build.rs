use std::path::Path;

fn list_files<P: AsRef<Path>>(dir: P) -> Vec<String> {
    let mut files = vec![];
    for entry in std::fs::read_dir(dir).unwrap() {
        let entry = entry.unwrap();
        let path = entry.path();
        if path.is_file() {
            match path.extension().map(|s| s.to_str().unwrap()) {
                Some("cpp") | Some("cxx") | Some("cc") | Some("c") | Some("hpp") | Some("inl") => {
                    files.push(path.to_string_lossy().to_string());
                }
                _ => (),
            }
        } else if path.is_dir() {
            files.extend(list_files(&path));
        }
    }
    files
}

fn main() {
    cc::Build::new()
        .include("engine/3rdparty/glfw-3.4/include")
        .files(list_files("engine/3rdparty/glfw-3.4/src"))
        .compile("glfw");
    println!("cargo:rustc-link-lib=glfw");

    // ! GG
    // cc::Build::new()
    //     .cpp(true)
    //     .include("engine/3rdparty/assimp-5.3.1/include")
    //     .include("engine/3rdparty/assimp-5.3.1/code")
    //     .include("engine/3rdparty")
    //     .include("engine/3rdparty/assimp-5.3.1/contrib/unzip")
    //     .include("engine/3rdparty/assimp-5.3.1") // for revision.h
    //     .include("engine/3rdparty/assimp-5.3.1/contrib/pugixml/src")
    //     .include("engine/3rdparty/assimp-5.3.1/contrib/rapidjson/include")
    //     .files(list_files("engine/3rdparty/assimp-5.3.1/code"))
    //     .compile("assimp");
    // println!("cargo:rustc-link-lib=assimp");

    cxx_build::bridge("src/main.rs")
        .std("c++17")
        // ? engine
        .include("engine/source")
        // .file("engine/source/runtime/main.cpp")
        .files(list_files("engine/source"))
        // ? shader
        .include("engine/shader/generated/cpp")

        // ? third parties
        .include("engine/3rdparty/glm-1.0.1-light")
        .file("engine/3rdparty/glm-1.0.1-light/glm/detail/glm.cpp")
        // .files(list_files("engine/3rdparty/glm-1.0.1-light"))
        .include("engine/3rdparty/glad/include")
        .files(list_files("engine/3rdparty/glad/src"))
        .include("engine/3rdparty/glfw-3.4/include")
        // ? assimp
        .include("engine/3rdparty/assimp-5.3.1/include")

        .include("engine/3rdparty")
        .include("engine/3rdparty/stb")
        .compile("cxx-demo");

    println!("cargo:rerun-if-changed=engine");
    println!("cargo:rerun-if-changed=main.rs");
    println!("cargo:rerun-if-changed=header.h");
}
