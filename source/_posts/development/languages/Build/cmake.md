---
title: CMake
categories:
- 开发
- 语言
- 构建 
tags:
category_bar: true
---

## cmake 快速使用
1. `touch CMakeLists.txt && mkdir build`
2. `cd build`
3. `cmake ..`
4. `make`
5. `./project # 运行可执行文件即可` 

## 变量
`CMAKE_CURRENT_BINARY_DIR`
二进制目录, build下目录

## 链接第三方库
* linux环境且库在系统默认路径下

```
find_package(Boost 1.84.0 REQUIRED) # 找库文件所在位置
target_link_libraries(client PRIVATE "${Boost_LIBRARIES}") # 链接静态库
```
* 库在windows环境或在linux的指定目录中

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
# 其它
PUBLIC: 本目标需要使用,依赖这个目标的目标也需要使用
INTERFACE: 本目标不需要使用,依赖这个目标的目标需要使用
PRIVATE: 本目标需要使用,依赖这个目标的目标不需要使用

XXXConfig.in.h中的cmakedefine和define区别:
define的无论值为什么都显示
cmakedefine在值为OFF/FALSE,0和未定义的变量时不会定义

XXXConfig.h和XXXConfig.in.h:
cmake对后者进行处理并拷贝到前者中

# 例子
Example(聊天室):
``` CMake
# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.8)
# 定义项目名称和版本
project(client VERSION 1.0 LANGUAGES CXX)

# 设置 C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 引入Boost库
# 手动辅助指定库位置
set(BOOST_ROOT "C:/environment dev/boost lib/boost")
# 手动辅助指定头文件位置
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include/boost-1_83/boost")
# 手动辅助指定静态库和动态库位置
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")
# 找库文件所在位置
find_package(Boost 1.83.0 REQUIRED COMPONENTS system)

# 添加源文件
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(SOURCES)
list(APPEND SOURCES "${SRC_DIR}/main.cpp")

# 判断
if(Boost_FOUND)
        message( STATUS "Boost FOUND !!!")
endif()

# 生成可执行文件
add_executable(client "${SOURCES}")

# 链接静态库
target_link_libraries(client PRIVATE "${Boost_LIBRARIES}")

# 为特定目标设置头文件目录
target_include_directories(client
        PUBLIC 
        "${CMAKE_SOURCE_DIR}/include"
        "${BOOST_INCLUDE_DIRS}"
)

# CMAKE_SOURCE_DIR 最外层CMakeLists.txt所在目录
# CMAKE_CURRENT_SOURCE_DIR 当前CMakeLists.txt所在目录

# 在build中 cmake ..  cmake --build . ./xxx.exe

# 打包静态库
# add_library(hello_library src/hello.cpp)
```
