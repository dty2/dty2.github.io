---
title: gcc
categories:
- 开发
- 开发工具
- GNU
tags:
- GNU
category_bar: true
---

## 介绍
GUN编译工具
-E: 预处理 (ps:需要使用-I指定头文件目录)

编译选项:
编译:
-S: 编译成.s
-pg: gprof选项
-I: 指定头文件目录
-c: 编译不链接
-o: filename 指定.o文件名
-v: 显示编译器调用的程序
-g: debug选项
-DNAME=VALUE: 定义宏,没有值时则默认为1 
-save-temps: 保存.i和.s文件

警告:
-Wall: woring all
-Wcomment 检测注释是否嵌套 更好的注释办法: 使用#if 0 ... #endif
-Wformat 使用scanf和printf时检查格式字符串是否匹配
-Wunused 检测未使用的变量
-Wimplicit 使用未声明函数
-Wreturn-type 函数类型不是void但是没有返回值
ps:以上是常见的警告

标准:
-ansi: 禁用gcc扩展
-pedantic: 严格遵守ISO C和ISO C++的标准
ps:不加则默认"方言"

链接:
-o 指定ELF文件名

优化:
o[0-3]: 三个等级,一般o0debug,o2投产即可
-funroll-loops: Loop Unrolling优化

source-level optimization:
CSE: 存储已经计算过的值来同时减少代码量和执行速度
FL: 内嵌函数,适用于经常被调用且代码量不多的函数
Loop Unrolling: 减少循环判断(空间换时间)

machine-level optimization:
scheduling: 新指令可在上一指令没结束前就可执行(前提是没有影响),在保证正确的情况下,最大化并行执行(提高速度,不增加空间,但是由于算法复杂耗费编译时间)

gcc直接将c++编译成汇编语言
大多数编译器都是将c++转化为c,再转化为汇编
链接c++ objectfiles时应使用g++,而不是gcc,因为使用gcc会连接c的标准库
很多c的编译链接选项也可用到c++的编译链接中
.cpp .cc .cxx都指代c++程序
.ii c++的".i"文件

