# config notes

* record some problem when I construct this project

## 1. CMake

* fking cpp

### 1.1 Missing OpenGL Lib

* Problem
  * ![image-20240304222207962](./config_notes/image-20240304222207962.png)
* Cause
  * link glfw but forget to OpenGL Lib
* Solution
  * add the following lines in CMakeLists.txt
  * ![image-20240304222948969](./config_notes/image-20240304222948969.png)

### 1.2 MacOS Issue

* Problem

  * ![543262b384a35b3a9cc45a0bc3bcf2f](./config_notes/543262b384a35b3a9cc45a0bc3bcf2f.png)

* Cause

  * ① Forget to invoke `glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);` in MacOS
  * ② MacOS only supports OpenGL <= 4.1

* Solution

  * ① Add the above sentence in MacOS

    ```c++
    #ifdef __APPLE__
        glfwWindowHint(...)
    #endif
    ```

  * ② Change OpenGL version to 4.1

### 1.3 Fucking CMake

* Problem

  * shader precompile cmake script cannot produce right result
  * Correct：![image-20240305232712403](./config_notes/image-20240305232712403.png)
  * Wrong：![image-20240305232911373](./config_notes/image-20240305232911373.png)

* Cause

  * cmake feature

* Solution

  * delete `VERBATIM`

  * ![image-20240305232954488](./config_notes/image-20240305232954488.png)

    > By ChatGPT4