use crate::ffi::main_fn;

#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {
        include!("bjtu-game-engine/src/header.h");

        fn main_fn();
    }
}

fn main() {
    println!("Hello, world! from Rust!");
    main_fn()
}
