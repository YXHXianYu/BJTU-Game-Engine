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