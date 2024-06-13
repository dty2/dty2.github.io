---
title: 配置arch
categories:
- linux
tags:
- arch
- 改键
- i3
sticky: 67
category_bar: true
index_img: /images/linux/arch/archlogo.jpeg
---
简介: 记录本人配置arch相关内容
<!-- more -->
## 配置文件
本人所有相关配置文件都在[dotfile](https://github.com/dty2/dotfile)中
## 桌面环境
{% note success %}
 人靠衣冠马靠鞍
{% endnote %}
桌面环境大体说可以有两种配置方式
* 折腾配置: 围绕窗口管理器进行DIY(类比vim)
* 简单配置: 现成的桌面环境如Gnome(类比IDE)
温馨提示: 折腾配置的费劲程度取决于你的需求程度
{% note danger %}
 配置下述软件极耗精力,需要查询官方文档以及各种资料
{% endnote %}
需要软件如下,详细作用在arch官网中可查

|     类别     |    软件名称   |
| ------------ | ------------- |
|  显示管理器  |  lightdm      |
|  窗口管理器  |  [i3](https://i3wm.org/docs/)           |
|  状态栏      |  [polybar](https://github.com/polybar/polybar/wiki)      |
|  效果混成器  |  picom        |
|  程序启动器  |  [rofi](https://github.com/davatorium/rofi?tab=readme-ov-file#manpage)         |
|  锁屏器      |  [i3lock-color](https://github.com/Raymo111/i3lock-color) |
|  壁纸管理器  |  [variety](https://peterlevi.com/variety/)      |
## 改键
### 改前须知
![Linux键盘工作原理](/images/linux/arch/kbd.png)
查看按下xorg的键位码: `xev`
查看xorg映射键位码: `xmodmap -pke | less`
查看xorg修饰键位: `xmodmap`
恢复xorg键位初始设置: `setxkbmap`
查看按下内核的键位码: `sudo showkey # 不加sudo就只能在TTY下执行`
### 相关工具
xbindkeys: 实现多个组合键执行一个命令,例如ctrl+i+j = `echo hello world`
xdotool: 模拟键位按压,例如按压j = 按压k
interception-tools: 基于evdev协议实现改键(不光能改键)
### 改键
本人参考这个[参考教程](https://zhuanlan.zhihu.com/p/404819427)
改键方式有很多种,由于本人使用vim,因此使用interception-tools进行改键
arch用户用yay安装`interception-dual-function-keys`(其他发行版本参考仓库地址编译)
将"caps"改为"ctrl + esc": 单独按下"caps"时为"esc"，按下"caps"+其它键位时为"ctrl"
```
# 添加/etc/interception/caps2ctrlesc.yaml文件，并在其中添加以下内容
TIMING:
    TAP_MILLISEC: 200 # 点击小于200毫秒判定为esc
    DOUBLE_TAP_MILLISEC: 150 # 可选配置

MAPPINGS:
    - KEY: KEY_CAPSLOCK
      TAP: KEY_ESC
      HOLD: KEY_LEFTCTRL
# 添加/etc/interception/udevmon.d/dual-function-keys.yaml，并在其中添加以下内容
- JOB: "intercept -g $DEVNODE | dual-function-keys -c /etc/interception/capslock2ctrlesc.yaml | uinput -d $DEVNODE"
  DEVICE:
    NAME: "AT Translated Set 2 keyboard" # 任务作用的设备
- JOB: "intercept -g $DEVNODE | dual-function-keys -c /etc/interception/capslock2ctrlesc.yaml | uinput -d $DEVNODE"
  DEVICE:
    NAME: "FILCO Bluetooth Keyboard"
```
如果想把改键配置放在其他路径, 记得修改上面配置相同的路径, 几个键盘设备就对应配置几个JOB
NAME参数的设备名称通过这个命令确定`sudo uinput -p -d /dev/input/by-id/X`, X是路径下类似keyboard名称的设备
如果这个路径下没有, 则需要在`/dev/input`目录下慢慢找了
配置写好后启动进程即可`systemctl enable --now udevmon.service`
