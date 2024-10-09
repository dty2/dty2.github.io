---
title: Arch Linux
---

Arch Linux
<!-- more -->

# Arch Linux

## 安装

### 坑

* live环境连不上网因为网卡被 [rfkill](https://wiki.archlinux.org/title/Network_configuration/Wireless#Rfkill_caveat)
* 创建分区用cfdisk(TUI界面,易用)
* 想要创建分区时发现没有发现空白分区是因为空白分区并不是块设备
* 已经有EFI分区的话,就不用再创建EFI分区
* 针对不同分区，格式化方式不同，挂载方式也不同，且挂载有顺序要求
* 挂在到/mnt目录是因为/mnt目录是你要安装的新系统的根目录
* [Arch源生成网站](https://archlinux.org/mirrorlist/)
* 基本包
  * linux-zen linux-zen-headers 内核与其头文件
  * base base-devel 最小软件包组base与其扩展
  * linux-firmware 固件包
  * intel-ucode 用于处理intel处理器微码更新的软件包
  * grub 启动引导程序
  * os-prober 用于探测磁盘中已存在的操作系统，为grub创建菜单项
  * efibootmgr 用于管理UEFI启动项的命令行工具
  * ntfs-3g 用于读写windows类别的ntfs格式的分区
  * dkms 用于动态内核模块支持系统，主要用于在内核升级时自动编译和安装第三方内核模块，确保与新系统兼容被nvidia-dkms依赖
  * xorg x11具体实现
  * vi(visudo需要) vim 编辑器与手册
  * networkmanager 网络管理
* 没有安装xorg包之前就关机会出现关机后电源灯还亮着且没真正关机这种情况,装完xorg解决问题
* 不安装驱动程序是因为通用驱动一般包含在内核内
* 内核包含的驱动程序是通用驱动,但是对于如英伟达这样的就需要.不同内核有不同安装英伟达驱动的方式,对于定制内核,比如本人使用了zen内核,就需要阅读 [这个](https://wiki.archlinuxcn.org/wiki/NVIDIA#%E5%AE%9A%E5%88%B6%E5%86%85%E6%A0%B8).对于具有intel和NVIDIA双显卡的笔记本需要阅读 [这个](https://wiki.archlinuxcn.org/wiki/NVIDIA_Optimus)

## 使用

### 帮助

`sudo pacman -S man man-pages`

### 资源监控

`sudo pacman -S neofetch htop fd`

* neofetch 显示系统信息
* htop 进程监控(Emacs操作)
* find -> fd 搜索

#### 显卡

[独立 + 集成](https://wiki.archlinuxcn.org/wiki/NVIDIA_Optimus)
`sudo pacman -S nvidia-dkms nvidia-utils xorg-xrandr`
`nvidia-smi`查看NVIDIA的使用状态

### 用户与提权

* `useradd -m -G wheel [username]` -m表示创建用户家目录，-G表示加入到用户组
* `passw [username]`
* 使用"visudo"配置"sudo"(即配置/etc/sudoers)
  * 删除"sudo"中“wheel”用户组前面的注释
  * 想要免密提权则在sudo配置文件按照下面格式添加内容

  ``` bash
  # Same thing without a password"
  # wheel ALL=(ALL:ALL) NOPASSWORD: ALL
  username ALL=(ALL:ALL) NOPASSWORD: /bin/vmware # <--- 添加这行等
  ```

  * 在最后填入`Defaults insults`可以实现口吐芬芳
* 杂项
  * wheel，可以使用sudo的用户组
  * 查看所有用户组`getent group`

### 服务

  ``` bash
  sudo systemctl enable/disable [--now] [service name] # 开启/关闭启动 [--now]立即开启/关闭
  sudo systemctl start/stop [service name] # 启动
  systemctl is-active/is-enabled/is-failed [service name] # 查看服务状态
  ```

* "service name"可以不用带后缀".service"，systemctl会默认添加

### 包管理工具

* 根据[arch镜像源生成网站](https://archlinux.org/mirrorlist/)选择合适的源，更改"/etc/pacman.d/mirrorlist"文件
* 使用[Pacman](https://wiki.archlinuxcn.org/wiki/Pacman)查找安装删除包

  ``` bash
  # 查找
  sudo pacman -Ss [package name] # 搜索软件包
  sudo pacman -Qi [package name] # 查看软件包详细信息
  sudo pacman -Qs [package name] # 搜索已经安装软件包
  sudo pacman -Sg [package name] # 列出本地软件仓库上的所有软件包组
  sudo pacman -Qg [package name] # 列出本地软件仓库上的所有软件包组和子软件包
  sudo pacman -Ql [package name] # 列出软件包所有文件安装路径
  # 安装
  sudo pacman -S [package name] # 安装软件包
  # 卸载
  sudo pacman -R [package name] # 只卸载该软件包不卸载它依赖的软件
  sudo pacman -Rs [package name] # 卸载该软件包和它依赖的软件
  sudo pacman -Rsc [package name] # 卸载该软件包和依赖它的软件
  ```

* [安装yay-bin](https://github.com/Jguer/yay?tab=readme-ov-file#binary)

### 字体

* `sudo pacman -S ttf-3270-nerd`
* `sudo pacman -S otf-font-awesome`
* [也字工厂曲奇体]()

### 亮度

* 安装`sudo pacman -S light`
* `sudo light -S/U/A [0-100]` 设置/降低/升高亮度

### 电源

* 安装`sudo pacman -S tlp`
* [tlp](https://linrunner.de/tlp/introduction.html)
  * 启动服务`sudo systemctl enable --now tlp`
  * `sudo tlp-stat -b`查看电量
  * 使用[tlp](https://linrunner.de/tlp/usage/index.html)

### 网络

* 安装`sudo pacman -S NetworkManager`(装系统时就要装，不装后续没有网无法用pacman安装NetworkManager)
* 使用[NetworkManager](https://wiki.archlinuxcn.org/wiki/NetworkManager)
  * 启动网络服务`sudo systemctl enable --now NetworkManager`
  * 查看网络状态`nmcli device wifi`
  * 连接网络`nmcli device wifi connect [name] password [password]`
  * [官网](https://networkmanager.dev/)
  * nmtui为其终端下图形界面
* 代理安装`yay -S v2raya-bin`
* 本地DNS解析文件是"/etc/resolv.conf"

### 蓝牙

* 安装`sudo pacman -S bluez bluez-utils`
* 启动蓝牙服务`sudo systemctl enable --now bluetooth`
* 使用蓝牙

  ``` bash
  bluetoothctl↩️
  power on # 开启蓝牙
  scan on # 扫描可用蓝牙
  connect [address/name] # 蓝牙连接
  ```

### 声音

* 安装`sudo pacman -S alsa-utils pulseaudio-alsa pulseaudio-bluetooth`
  * alsa-utils包含底层音频驱动控制等的工具包，alsa是内核级组件
  * pulseaudio声音服务器，中间件提供高级服务
  * 光安装alsa是不够的，一些音频设备比如我的WH-1000XM3就无法使用

### 键盘

#### 改键

* 改键
  1. 安装`yay -S interception-dual-function-keys`
  2. 配置文件/etc/interception/caps2ctrlesc.yaml，添加以下内容

    ``` sh
    TIMING:
      TAP_MILLISEC: 200 # 点击小于200毫秒判定为esc
      DOUBLE_TAP_MILLISEC: 150 # 可选配置
    MAPPINGS:
      - KEY: KEY_CAPSLOCK
        TAP: KEY_ESC
        HOLD: KEY_LEFTCTRL
    ```

  3. 配置文件/etc/interception/udevmon.d/dual-function-keys.yaml，添加以下内容

``` sh
# 注意JOB中的文件路径，几个键盘设备就几个JOB
# NAME参数的设备名称通过这个命令确定`sudo uinput -p -d /dev/input/by-id/X`, X是路径下类似keyboard名称的设备
# 如果这个路径下没有, 则需要在`/dev/input`目录下慢慢找了
- JOB: "intercept -g $DEVNODE | dual-function-keys -c /etc/interception/capslock2ctrlesc.yaml | uinput -d $DEVNODE"
  DEVICE:
    NAME: "AT Translated Set 2 keyboard" # 任务作用的设备
- JOB: "intercept -g $DEVNODE | dual-function-keys -c /etc/interception/capslock2ctrlesc.yaml | uinput -d $DEVNODE"
  DEVICE:
    NAME: "FILCO Bluetooth Keyboard"
```

  4. 启动进程`sudo systemctl enable --now udevmon.service`

* [改键参考](https://zhuanlan.zhihu.com/p/404819427)
* 杂项
  * 查看按下xorg的键位码: `xev`
  * 查看xorg映射键位码: `xmodmap -pke | less`
  * 查看xorg修饰键位: `xmodmap`
  * 恢复xorg键位初始设置: `setxkbmap`
  * 查看按下内核的键位码: `sudo showkey # 不加sudo就只能在TTY下执行`
  * [interception-tools](https://wiki.archlinux.org/title/Interception-tools)控制且自定义键盘输入映射

#### 输入法

* 安装[fcitx5](https://wiki.archlinuxcn.org/wiki/Fcitx5)
  * `sudo pacman -S fcitx5-im fcitx5-chinese-addons fcitx5-qt fcitx5-pinyin-sougou fcitx5-gtk fcitx5-nord`
  * `yay -S fcitx5-input-support`
* 使用图形界面"fcitx5-configtool"进行配置

#### 剪贴板

* 安装`sudo pacman xclip`

#### 鼠标换皮 -->TODO

### 桌面环境

#### 显示管理

lightdm

#### 窗口管理

[i3](https://i3wm.org/docs/)

#### 状态栏

[polybar](https://github.com/polybar/polybar/wiki)(弃用)

* `yay -S lemonbar-xft-git`
* 配置相关工具与应用
  * wmctrl查询窗口状态
  * xbindkeys操作转换成命令
  * xdotool命令模拟操作
  * xprop WM_CLASS 用于picom等

#### 效果混成

picom

#### 程序启动

[rofi](https://github.com/davatorium/rofi?tab=readme-ov-file#manpage)

#### 锁屏

[i3lock-color](https://github.com/Raymo111/i3lock-color)

#### 壁纸管理

[variety](https://peterlevi.com/variety/)

### 日常

1终端模拟器 [kitty]()
2终端模拟器 [alacritty](https://alacritty.org/config-alacritty.html)
Shell zsh
浏览器 edge
截屏 flameshot
即时通讯 qq
文件浏览器 ranger
命令行辅助器 tldr

#### 虚拟化 vmware

* 网络连接
  * 桥接: 直接连接到物理网络,外部网络可以访问
  * NAT转换: 虚拟机共享主机IP访问外部网络，外部网络不可访问虚拟机
  * Host-only：主机模式，只与主机网络通讯
* 第一次使用需要`sudo systemctl enable --now vmware-networks`开始网络
* 若没发现vmware-tools正在安装，那么在客户机中的虚拟CD-ROM驱动器上运行"setup.exe"进行安装
* 启用共享文件夹需要勾选"map as a network drive in windows guests"
容器 docker
密码管理器 pass

### 办公

办公 feishu wemeet
流程图 drawio

### 多媒体

图片处理器 gimp
图片查看器 feh
截屏 flameshot
终端录屏 asciinema
