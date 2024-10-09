---
title: 工具
---

工具使用备忘
<!-- more -->

# 工具

## gdb

```gdb
(gdb) c
(gdb) fork
(gdb) set follow-fork-mode child/parent
(gdb) c
(gdb) info inferiors
(gdb) inferiors <process-id>
(gdb) c

(gdb) layout asm
(gdb) layout src
(gdb) layout split
(gdb) si # 执行下一条汇编指令

(gdb) list 
(gdb) list <function name>
(gdb) list <line number>

(gdb) disassemble
(gdb) disassemble <function name>

(gdb) catch # 捕捉事件发生，比如系统调用，动态库加/卸载，抛异常
(gdb) search # 搜索代码
```

* 查看main函数执行之前的backtrace需要在gdb内进行设置`set backtrace past-main on`
  * [How to backtrace how main's called with gdb?](https://stackoverflow.com/questions/5461731/how-to-backtrace-how-mains-called-with-gdb)
  * [backtrace](https://sourceware.org/gdb/current/onlinedocs/gdb.html/Backtrace.html)

## gcc

``` bash
# 制作静态库
gcc -c add.c -o add.o
ar rcs libadd.a add.o # rcs 插入文件,创建库,写入索引

# 制作动态库
gcc -fPIC -c sub.c -o sub.o # -fPIC 生成位置无关代码
gcc -shared -o libsub.so sub.o # -shared 生成共享库

# 编译
# -no-pie不采用PIC -save-temps保存中间文件, -Wl,-rpath指定运行时库查找路径
gcc -no-pie -save-temps -o calculate calculate.c -L. -ladd -L. -lsub -Wl,-rpath=.
```

## 反汇编

* readelf: 查看ELF文件结构信息工具
* hexdump: 查看ELF文件原始字节数据工具
* objdump: 查看反汇编,显示符号表与节等内容

``` bash
readelf -W -a a.out | less # -W禁用折叠 -a查看全部内容
objdump -d a.out | less # -d 显示执行节的汇编内容
objdump -D # 是将所有的段都进行反汇编,无论是数据还是指令
hexdump -C a.out | less # -C 标准16进制+ASCLL显示
```

## bear

不用cmake就能生成compile_commands.json给clangd

``` bash
bear -- make
bear -- make / cmake / ninja
```

## git
