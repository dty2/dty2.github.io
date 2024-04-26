---
title: 命令备忘录
categories:
- 计算机
- 操作系统
- linux
tags:
- Arch日记
category_bar: true
---
## 基本命令
## sudo
简介: 提权工具
需要将想使用sudo命令的用户添加到sudoers文件或sudo组中
`usermod -aG sudo username`
## 其它命令
## nmcli
networkmanager管理网络
```
sudo systemctl restart NetworkManager #重启
```
## hexo博客
1. 博客启动命令 `docker exec -it blog sh`
2. 博客在docker的使用
```
hexo clean # 清空缓存
hexo generate # 生成静态页面
hexo deploy # 部署
```
外界需要用git进行同步更新和push
# git
用于版本控制
```
# git 注册
git config --global user.name "hunter"
git config --global user.email "284050500@qq.com"
git rm -rf --cached . # 清除git缓存
# 用于在.gitignore 添加了新内容(之前不在.gitignore中却已经提交过)之后不生效
```
# tree
目录树
```
tree -d # 只显示目录
tree -L x # 显示到x级目录
```
# tldr
too long don't read 命令快速入手
`tldr --update #刚下载的话需要更新`
# ldd
查看可执行文件依赖哪些动态库
# nm
查看符号表
用于检查某个函数是否在某个目标文件中被定义
T: 函数被定义
U: 函数未被定义(动态库)
# file
用于显示文件类型
可执行文件:
linux: .out/ELF(Executable and Linking Format)
MS-DOS: COFF(Common Object File Format)
windows: .EXE(Execute)
静态库:
linux: .a (archive存档)
动态库:
linux: .so (shared object)
windows: .dll
LSB/MSB: 低位在前,高位在前 cpu理解字节顺序的方向, LSB:interl和AMDx86 MSB:Motorola
not stripped: 包含符号表,包含debug信息
# time
计算程序运行时间
```
time ./a.out
real: xxx
user: 用户态
sys: 内核态
```
# fc-list
显示当前系统所有可使用的字体
/usr/share/fonts/pc/Px_IBM_3270pc.ttf: Px IBM 3270pc:style=Regular
想使用以上字体,填入Px IBM 3270pc(即":"后的内容)
# ulimit: 
`ulimit -c`查询是否允许产生core文件, 0不允许, unlimit不受限制(允许)
有关core dump(核心转储)文件(即程序崩溃之后产生的尸体,记录其在内存中的那时候的状态)
# useradd 和 adduser
useradd 低级命令，具体需要指定具体内容
adduser 高级命令(小白使用)
# passwd
指定root密码
# 常见文件 
.sudo_as_admin_successful: 记录使用sudo时是否成功获得root权限，成功获得会在主目录创建该文件
.zcompdump: 记录zsh中执行的命令，由zsh的compinit机制创建，加速命令补全
.motd_shown: 显示MOTD(message of the day)信息
.lesshst: 记录在less中执行的命令
.profile: 是sh和bash默认的配置文件。如果使用的是bash则可以在其中设置环境变量。如何使用zsh则应该在zshrc中设置环境变量。该文件会在用户登录的时候执行一次。如果对其更改，需注销后重新登录才会生效。
