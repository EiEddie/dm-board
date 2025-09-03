# 组件

模块化地为项目提供更多可用功能.


## 使用
要在主项目中使用组件, 只需在根目录 `CMakeLists.txt` 里添加:
```cmake
# Include Apps
add_subdirectory(App)
# Import the dependencies
target_link_libraries(app PUBLIC
  bsp m
  # Components
  comp::<name> # 在此处添加
)
message(STATUS "Apps imported")
```

并在主项目对应位置按如下格式导入头文件:
```c
#include <comp/{name}/{head}>
```
将 `{name}` 替换为组件名字, `{head}` 替换为相应的头文件名.


## 自定义组件
添加自定义的新组件十分简单, 你只需要:

1. 新建目录 `Components/<dir>`, 用于放置自定义组件. \
  这一目录名 `<dir>` 不强制要求与组件名 `<name>` 相同, 但**建议**这样做.
2. 在目录 `<dir>` 下新建 `include` 与 `src` 两个目录, 用于放置**对外**头文件与源代码. 在这两个目录内可以编写你的代码.
3. 在 `<dir>` 内新建一个 `CMakeLists.txt`, 内容模板如下:
```cmake
# 将所有 <name> 替换为你的组件名
# 共有 6 处需要替换, 请仔细核对

add_library(comp_<name> STATIC
  src/... # 此处放置你的源代码文件
)

target_include_directories(comp_<name>
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/comp/<name>>
)

target_link_libraries(comp_<name> PUBLIC comp_public_includes)
# 此处可以编写依赖说明

add_library(comp::<name> ALIAS comp_<name>)
```

这样就完成了新组件的添加, 重新配置 (configure) 项目即可使用.
**每次修改头文件, 都需要重新配置**.
关于组件约定与互相依赖, 请看接下来几节.


### 组件依赖
如果你编写的组件 `<name>` 依赖已经存在的组件 `comp1`, 需要在模板指定的位置添加
```cmake
target_link_libraries(comp_<name> PUBLIC comp::comp1)
```
如果有更多依赖项, 将其逐个按上述方法列出即可.


### 组件约定
- 组件实现的功能应该与硬件细节**完全分离**, 这也就意味着组件代码不要包含任何底层提供的 ABI.
  若要使用来自硬件的信息, 可以在对外暴露的 ABI 中作为参数传入.
- 关于命名约定:
  - 组件名 `<name>`, 组件头文件标识名与目录名 `<dir>` 可以互不相同, 但强烈建议**不要**这么做. 除非你有足够合理的理由. \
    头文件标识名为 `$<INSTALL_INTERFACE:include/comp/<name>>` 中的 `<name>`,
    用于在外部调用组件时起到说明的作用.
  - 外部调用组件时使用的名字有 `comp::<name>` 与 `comp_<name>` 两种, 其中前者是后者的别名.
    强烈建议**不要**更改此命名规则.


## 实现细节
### 头文件搬运
按照 `c/c++` 的一般头文件引用规则, 需要的目录为 `Components/<name>/include/<head>` 或它的字串.
为了具备*自解释性*, 我们希望使用形如 `comp/<name>/<head>` 的方式引用, 但是又不想破坏现有的目录结构,
例如在 `include` 目录中添加几个层级.

这实际上是一个两难的问题, 最终采取的方案是:
将每个组件的 `include` 目录中的所有文件都复制到构建目录中规划好的位置.
因此引用时使用的头文件其实存在于构建目录中某处, 而非 `Components` 目录.
复制过程每次配置 (configure) 时进行一次, 因此修改头文件需要重新配置.
