# Scripts

- `glsl-preprocessor.py`: glsl 预处理器（处理 `#include`）

## 1. glsl-preprocessor

### 功能

将指定目录 `shaders_root` 目录及其所有字目录下的着色器文件进行预处理（处理 `#include "xxx"`），并输出到 `shaders_root/build/` 目录下。

> 找不到 include 目标文件时或出现循环引用时报错

### 参数

`REL_ROOT`：为 `shaders_root` 相对于 `glsl-preprocessor.py` 的相对路径。

`EXTS`：需要进行预处理的文件的扩展名

> 指定目录的解析不依赖于当前工作目录，只依赖于脚本文件位置。
