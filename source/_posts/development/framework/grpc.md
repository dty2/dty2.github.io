---
title: grpc 
categories:
- 开发
- 框架
tags:
- grpc
category_bar: true
---

## grpc
grpc可以说是很多库的集合体

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
