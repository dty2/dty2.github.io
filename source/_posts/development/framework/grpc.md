---
title: grpc 
categories:
- 开发
- 框架
tags:
- grpc
sticky: 98
category_bar: true
index_img: /images/3.jpg
---
grpc使用备忘
<!-- more -->
## 快速使用
{% note info %}
示例在最后
{% endnote %}
### 安装与构建
以下是在ubuntu中安装grpc的方式，最新版本以及依赖参照最新版官方文档
参考[CPP版官方文档](https://grpc.io/docs/languages/cpp/quickstart/)
#### 安装
```
export INSTALL_DIR=$HOME/lib
mkdir $INSTALL_DIR`
export PATH="$INSTALL_DIR/bin:$PATH"`
sudo apt install cmake`
sudo apt install -y build-essential autoconf libtool pkg-config`
git clone --recurse-submodules -b v1.62.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc`
```
#### 构建
```
cd grpc
mkdir -p cmake/build
pushd cmake/build
cmake -DgRPC_INSTALL=ON \
    -DgRPC_BUILD_TESTS=OFF \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
    ../..
make -j 8
make install
popd
```
### 使用
以下内容详细代码见[示例](https://github.com/grpc/grpc/tree/master/examples)
#### 构建
在grpc的examples中可以发现`common.cmake`文件
该文件中有几种找到grpc库并构建的方式
以下则是截取最后一种，将其合并到了CMakelist.txt中
```
project(helloworld C CXX)
cmake_minimum_required(VERSION 3.8)

# 优化编译什么的，在交叉编译时尤为有用
# option(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(REFLECTION gRPC::grpc++_reflection)
set(PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(GRPC_GRPCPP gRPC::grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

# Proto file
get_filename_component(proto "./helloworld.proto" ABSOLUTE)
get_filename_component(proto_path "${proto}" PATH)

# Generated sources
set(proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.pb.cc")
set(proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.pb.h")
set(grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.cc")
set(grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h")

# 使用proto生成.cpp和.hpp文件 
add_custom_command(
      OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
      COMMAND ${PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${proto}"
      DEPENDS "${proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# grpc_proto
add_library(grpc_proto
  ${grpc_srcs}
  ${grpc_hdrs}
  ${proto_srcs}
  ${proto_hdrs}
)

target_link_libraries(grpc_proto
  ${REFLECTION}
  ${GRPC_GRPCPP}
  ${PROTOBUF_LIBPROTOBUF}
)

foreach(target
  client server)
  add_executable(${target} "${target}.cc")
  target_link_libraries(${target}
    grpc_proto
    absl::flags
    absl::flags_parse
    ${REFLECTION}
    ${GRPC_GRPCPP}
    ${PROTOBUF_LIBPROTOBUF})
endforeach()
```
#### 编写proto文件
* 编写service部分
* 编写message部分
#### 编写同步server文件
* 继承并实现server
* 创建"服务构建对象"
* 通过"服务构建对象“注册服务
* 通过"服务构建对象“开启监听
* 创建服务并传入"通过服务构建对象"的构建并开始函数
* 等待连接
#### 编写同步client文件
* 创建频道与存根
* 将存根与频道绑定
* 定义消息
* 创建客户端上下文
* 进行远程调用，捕获状态
* 返回状态判断
#### 编写异步server文件
* 创建"服务构建对象"
* 通过"服务构建对象“注册服务并且开启监听
* 创建服务并传入"通过服务构建对象"的构建并开始函数
* 等待连接
#### 编写异步client文件

## Abseil
```
ABSL_FLAG(type, name, default_value, "explain"); # 定义name的值
absl::ParseCommandLine(argc, argv); # 解析命令行参数
absl::GetFlag(FLAGS_name); # 获取FLAGS_port的值，其中name对应ABSL_FLAG中的name

# 当命令行参数中使用参数 --name=xxx
# 那么xxx这个值就会被ParseCommandLine解析为name的值
# 最后FLAGS_name的值就为name的值也就是xxx

std::string xxx = absl::StrFormat("0.0.0.0:%d", port); # 格式化字符串，支持std::string
```
