---
title: CMakeLists
categories:
- 开发
- 语言
- 构建 
tags:
- cmake
category_bar: true
---
简介: CMakeLists.txt语法笔记
<!-- more -->
## cmake 快速使用
{% note info %}
示例在最后
{% endnote %}
* `touch CMakeLists.txt && mkdir build`
* `cd build`
* `cmake -DCMAKE_INSTALL_PREFIX .. # 默认安装到系统下`
* `make`
* `./project # 运行可执行文件即可` 
## 链接第三方库
### Linux环境
* 若在指定目录，则需要更改环境变量等手动指定的方式来链接
* 库在系统默认路径下使用find_package即可

```
find_package(Boost 1.84.0 REQUIRED) # 找库文件所在位置
target_link_libraries(client PRIVATE "${Boost_LIBRARIES}") # 链接静态库
```
### Windows环境
```
# 手动指定库位置
set(BOOST_ROOT "C:/environment dev/boost lib/boost")
# 手动指定头文件位置
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include/boost-1_83/boost")
# 手动指定静态库和动态库位置
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")
# 链接
target_link_libraries(client PRIVATE "${Boost_LIBRARIES}") # 链接静态库
```
## 其它
### 变量
`CMAKE_CURRENT_BINARY_DIR`当前cmakelist目录
`CMAKE_SOURCE_DIR`最外端的cmakelist目录
### 参数
PUBLIC: 本目标需要使用,依赖这个目标的目标也需要使用
INTERFACE: 本目标不需要使用,依赖这个目标的目标需要使用
PRIVATE: 本目标需要使用,依赖这个目标的目标不需要使用

XXXConfig.in.h中的cmakedefine和define区别:
define的无论值为什么都显示
cmakedefine在值为OFF/FALSE,0和未定义的变量时不会定义

XXXConfig.h和XXXConfig.in.h:
cmake对后者进行处理并拷贝到前者中
