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

### 1.4 Texture All Black

* Problem
  * ![image-20240306213512906](./config_notes/image-20240306213512906.png)
* Cause
  * C++析构函数用不好，导致资源被提前析构
  * RenderTexture类析构函数
    * ![image-20240306224149455](./config_notes/image-20240306224149455.png)
  * 初始化构造资源
    * ![image-20240306224217965](./config_notes/image-20240306224217965.png)
  * 这就导致：初始化构造资源时，就会调用RenderTexture析构函数，从而释放资源
  * **找了两个多小时！！！太痛苦了！**
* Solution
  * ① 不用析构函数了，自己重载一套（比如Unreal）
  * ② 质疑C++、理解C++、成为C++！使用C++特性 —— 智能指针，通过 `shared_ptr` 来管理RenderTexture等RAII资源。这样就可以在拷贝时只拷贝指针，避免析构函数被调用！

### 1.5 glGetProgramiv参数错误

* Problem
  * 在MacOS上，shader代码无误，但是程序报Program compile error
    * 并且MacOS上，没有编译的错误信息
    * 同一份代码，Windows系统上，编译正常
  * ![9c97abb903153b20d2189e16ab9f13c](./config_notes/9c97abb903153b20d2189e16ab9f13c.jpg)
* Cause
  * 参数写错了
  * 错误：![image-20240307194842775](./config_notes/image-20240307194842775.png)
  * 正确：![image-20240307194856641](./config_notes/image-20240307194856641.png)
* Solution
  * 改成GL_LINK_STATUS就行了