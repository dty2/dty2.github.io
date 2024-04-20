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
简介: 记录grpc相关的学习内容
<!-- more -->
## 帮助文档
* [grpc官网文档](https://grpc.io/)
* [grpc官方github的doc](https://github.com/grpc/grpc/tree/master/doc)
* [grpc官方github的proposal](https://github.com/grpc/proposal)
* [zhangxiaoan的博客](http://www.anger6.com/categories/%E5%88%86%E5%B8%83%E5%BC%8F/)
* [Jarle's blog](https://lastviking.eu/fun_with_gRPC_and_C++/index.html)
## FAQ
### 使用场景
grpc多用于服务器间通讯
也适用于将设备、移动应用程序和浏览器连接到后端服务
但是由于其浏览器兼容性，接口严格，protobuffer的可读性差导致较少使用于连接前后端
### 安装建议
个人建议按照官方建议将grpc组件集中安装在某一处后通过环境变量来访问
### 使用建议
对于同步和异步的选择标准
* 想要简单，就选择同步
* 想要简单还想要效率，选择回调异步
* 想要细致控制就选CQbase异步
## 库与API
### grpc
```
# 服务健康检查
grpc::EnableDefaultHealthCheckService(true);
# 构建反射服务
grpc::reflection::InitProtoReflectionServerBuilderPlugin();
```
### Abseil
```
ABSL_FLAG(type, name, default_value, "explain"); # 定义name的值
absl::ParseCommandLine(argc, argv); # 解析命令行参数
absl::GetFlag(FLAGS_name); # 获取FLAGS_port的值，其中name对应ABSL_FLAG中的name

# 当命令行参数中使用参数 --name=xxx
# 那么xxx这个值就会被ParseCommandLine解析为name的值
# 最后FLAGS_name的值就为name的值也就是xxx

std::string xxx = absl::StrFormat("0.0.0.0:%d", port); # 格式化字符串，支持std::string
```
