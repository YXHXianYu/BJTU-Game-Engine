# config notes

* record some problem when I construct this project

## State 1

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

## State 2

### 2.1 延迟渲染

* 盲写写了半天，把前向渲染一口气改成延迟渲染了
* Problem
  * preview：恐怖游戏渲染
  * ![image-20240422223611841](./config_notes/image-20240422223611841.png)
* Cause1
  * 修改架构的时候，location没对上，一个3写成4了
    * 错误：![image-20240422224958148](./config_notes/image-20240422224958148.png)
    * 正确：![image-20240422225007403](./config_notes/image-20240422225007403.png)
* Cause2
  * 人物模型进行Model Transform的时候，法线也需要一起变换！我忘记了，所以人物的法线是错误的（导致人物很暗）
  * 错误效果
    * ![image-20240422231048211](./config_notes/image-20240422231048211.png)
    * 可以注意到，背后旋转的方块，法线的方向是错误的！
  * 法线是向量，不能和点一样直接进行Model Transform，需要得到一个特殊的Normal Transform
  * $M_{normal} = ((M_{model})^{-1})^{T}$ 
  * 证明见我笔记
  * 正确效果
    * ![image-20240422235054359](./config_notes/image-20240422235054359.png)
* Cause3
  * 画面偏灰（对比度低）
  * 错误效果
    * ![image-20240423000942111](./config_notes/image-20240423000942111.png)
  * 原因是ambient没有考虑kd，所以相当于整个画面变亮，对比度就降低了，画面不好看
  * 正确效果
    * ![image-20240423002521650](./config_notes/image-20240423002521650.png)
* Cause4
  * 高光怎么写都不对
  * 后面法线高光好像反了，你要和光源站在一起，才会出现高光
  * 后面发现GLSL的 **reflect** 函数的第一个参数是 表面点指向光源向量的 **负值**！
    * 竟然是负值！
  * 改完就对了，找了好久
* Cause...
  * 一大票错误
* 终于！
  * ![ba68fcd09ee318a8723345b43264037](./config_notes/ba68fcd09ee318a8723345b43264037.png)