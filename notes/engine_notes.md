# BJTU-Game-Engine

## 1. Render Module

### 1.1 类的结构

* 类的结构为如下树状结构
  * WindowSystem：管理窗口（单一）
  * RenderSystem：管理整个渲染模块（单一）
    * RenderResource：管理资源（单一）
      * RenderEntity：表示一个渲染实体，通过uuid在map中查找
        * RenderMesh：表示一个Mesh，通过整数下标查找
        * RenderTexture：表示一个纹理，通过整数下标查找
    * RenderPipeline：管理渲染管线（单一）
      * RenderShader：表示一个着色器，通过uuid在map中查找
* RenderResource管理资源数据、RenderPipeline管理渲染数据
* 同时，所有具体的渲染的操作，都在RenderPipeline中执行
  * RenderSystem只负责调用RenderPipeline，并将RenderResource传入Pipeline

### 1.2 注意事项

* 类的类别
  * 注意，在Render Module中，类的初始化默认不在构造函数中实现，而在 `initialize` 函数中实现
    * 同理，类的销毁也不在析构函数中实现，而在 `clear` 函数中实现
  * ① 需要显示初始化和销毁的类
    * RenderSystem, WindowSystem, RenderResource, RenderPipeline
    * 通常为高层级的类
  * ② 完整生命周期管理的类（遵循RAII）
    * RenderMesh, RenderTexture, RenderShader
    * 通常为底层级的类，每个类管理一类资源

