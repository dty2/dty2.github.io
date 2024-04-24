---
title: gcov
categories:
- 开发
- 开发工具
- GNU
tags:
- GNU
category_bar: true
---

## 介绍
查看程序中每一行执行的时间,用于优化
使用前gcc编译需加-fprofile-arcs -ftest-coverage参数
编译之后会产生新文件:xxx.gcno
运行之后会产生新文件:xxx.gcda
使用gcov xxx.c后再次产生新文件xxx.c.gcov
打开xxx.c.gcov可看见每行执行次数

