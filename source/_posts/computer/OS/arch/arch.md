---
title: Arch日记
categories:
- 计算机
- 操作系统
- linux
tags:
- Arch日记
sticky: 99
category_bar: true
index_img: /images/2.jpg
---
记录本人Arch的从安装到使用全过程
<!-- more -->
{% note danger %}
详细内容请看官方文档和第三方辅助文档
以下内容仅为记录和个人参考，不保证正确
{% endnote %}
## 帮助
* [官方安装指南](https://wiki.archlinuxcn.org/wiki/安装指南)
* [第三方辅助文档1](https://archlinuxstudio.github.io/ArchLinuxTutorial/#/)
* [第三方辅助文档2](https://arch.icekylin.online/)
* [参考视频](https://www.bilibili.com/video/BV1fk4y1w7wq/?spm_id_from=333.999.0.0&vd_source=8836eda798f42e634172036484104534https://arch.icekylin.online/)
## 序
Linux版本千千万，为何独选Arch？
* 可定制化程度高
* 有相当完善的帮助文档
* 有庞大的软件仓库
* 采用滚动更新
* Arch社区很友好
* ~~听说Arch安装困难，想装一个用来装逼~~
## 安装Arch
### 安装前
#### 准备引导启动盘
具体内容见{% post_link Kali安装 %}
虽然制作的是kali的启动盘，但是流程大体相似
{% note info %}
为什么这里放置的是制作kali的启动盘呢？
因为当初我最开始在真机上安装的Linux版本是Kali并记录下了笔记到制作Arch的启动盘的时候，就懒得写笔记了，反正都大差不差。置于为什么是Kali，懂的都懂
{% endnote %}
#### 进入引导系统后
{% note info %}
手机可以访问arch官网，推荐一边看手机，一边进行安装
{% endnote %}
* 连不上网可能是因为网卡被[rfkill](https://wiki.archlinux.org/title/Network_configuration/Wireless#Rfkill_caveat)
* 创建分区建议使用cfdisk(别问为什么，问就是简单易用)
* 想要创建分区时发现没有发现空白分区是因为空白分区并不是块设备
* 已经有EFI分区的话，就不用再创建EFI分区
* 格式化(针对不同分区，格式化的方式不同)
* 挂载(针对不同分区，挂载的方式不同)
* 挂在到/mnt目录是因为/mnt目录是你要安装的新系统的根目录
### 安装中
* 选择镜像站

```
# 国内安装需要国内源
Server = https://mirrors.ustc.edu.cn/archlinux/$repo/os/$arch
Server = https://mirrors.tuna.tsinghua.edu.cn/archlinux/$repo/os/$arch

# 国外源 建议速度配置代理
Server = https://mirror.archlinux.tw/ArchLinux/$repo/os/$arch   #东亚地区:中华民国
Server = https://mirror.0xem.ma/arch/$repo/os/$arch    #北美洲地区:加拿大
Server = https://mirror.aktkn.sg/archlinux/$repo/os/$arch    #东南亚地区:新加坡
Server = https://archlinux.uk.mirror.allworldit.com/archlinux/$repo/os/$arch    #欧洲地区:英国
Server = https://mirrors.cat.net/archlinux/$repo/os/$arch    #东亚地区:日本
```
* 别忘记重新安装密钥环，`pacman -S archlinux-keyring`。密钥环用于验证软件包，若不更新可能会造成密钥信任问题

* 盘点安装所需包

```
linux-zen linux-zen-headers base base-devel linux-firmware grub os-prober efibootmgr ntfs-3g intel-ucode dkms xorg vim man bluez bluez-utils networkmanager nvidia-dkms
```
{% note danger %}
以上包不一定为必须内容，可以进新系统后再装
不过，对于某些特殊的包，比如网络管理软件的包networkmanager就一定要安装
对于那些[不装networkmanager](#未安装netmanager)的Arch新人，我送你四个字"生死难料"
{% endnote %}
* 解释安装所需包
  - linux-zen是内核(zen是什么？详见{% post_link 操作系统常识 %})
  - linux-zen-headers是内核头文件
  - base是最小软件包集合
  - base-devel是软件包组base的扩展
  - linux-firmware是固件包
  - grub是启动引导程序
  - os-prober用于探测磁盘中已存在的操作系统，为grub创建菜单项
  - efibootmgr用于管理UEFI启动项的命令行工具
  - ntfs-3g用于读写windows类别的ntfs格式的分区
  - intel-ucode用于处理intel处理器微码更新的软件包
  - dkms用于动态内核模块支持系统，主要用于在内核升级时自动编译和安装第三方内核模块，确保与新系统兼容被nvidia-dkms依赖
  - xorg用于x11的实现
* 安装xorg包之前就关机会造成电源灯并不关闭并且没有真正关机，只是单纯黑屏
* 不安装驱动程序是因为通用驱动一般包含在内核内
* 注意，内核包含的驱动程序是通用驱动，但是对于如英伟达这样的就需要[特别安装驱动](#NVIDIA)
### 安装后
* 零碎操作备忘

```
genfstab -U /mnt >> /mnt/etc/fstab # 自动挂载配置文件
ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime #设置时区
# 删除/etc/locale.gen中en_US.UTF-8 和 zh_CN.UTF-8 UTF-8 前的#
locale-gen # 生成locale
# 编辑/etc/locale.conf 添加LANG=en_US.UTF-8
# 编辑/etc/hostname 添加主机名称
passwd root # 设置root的密码
useradd -m -G wheel [username] # 创建普通用户
passwd [username] # 设置username的密码
# 编辑sudoers赋予用户root的sudo权限，删除%wheel ALL=(ALL:ALL) ALL前的#
# 用systemctl 启用蓝牙和网络服务
# 编辑/etc/default/grub，去掉GRUB_DISABLE_OS_PROBER=false前的#
# 安装grub服务
grub-install --target=x86_64-efi --efi-directory=/efi --bootloader-id=grub # 其中参数是可以改变的，详细内容看官方文档，参数错误就会造成grub安装位置错误
grub-mkconfig -o /boot/grub/grub.cfg # 更新grub引导
```
* 设置时区之后使用[NTP](https://en.wikipedia.org/wiki/Network_Time_Protocol)客户端使系统时间同步网络时间
* [语言本地化](https://wiki.archlinuxcn.org/wiki/%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87%E6%9C%AC%E5%9C%B0%E5%8C%96)建议遵循建议在.xprofile中进行设置
### 安装结束
这时，可以重启Arch，并拔掉引导盘了
如果一切[顺利](#grub相关)的话，电脑重启会进入grub，在那里，你可以找你的Arch系统和Windows系统。
然后登进Arch系统，输入用户名与密码
待以上内容都完成后
恭喜！到了这里安装Arch可以说是基本结束了，剩下的就开始配置Arch了
## 配置Arch
我的所有相关配置文件都在[这里](https://github.com/dty2/dotfile)
### 桌面环境
{% note success %}
人靠衣冠马靠鞍 - 桌面环境
{% endnote %}
桌面环境大体说可以有两种配置方式
* 折腾配置：以窗口管理器为核心进行DIY
* 简单配置：现成的桌面环境如Gnome  
#### 折腾配置
配置策略: 高效实用，简洁
  1. 全键盘流工作方式，辅之必要的鼠标操作
  2. 最大化利用桌面
  3. 状态栏干净，对不必要的信息进行收纳
  4. 不花里胡哨，界面以干净为主，减少不必要的特效

针对以上原则进行以i3wm为核心进行DIY
* 显示管理器: lightdm lightdm-slick-greeter(登陆界面) 
* 窗口管理器: [i3](https://i3wm.org/docs/)
* 状态栏: [polybar](https://github.com/polybar/polybar/wiki)
* 效果混成器: picom 增强版picom-animations(在官方配置上微调即可)
* 程序启动器: [rofi](https://github.com/davatorium/rofi?tab=readme-ov-file#manpage)
* 锁屏器软件: [i3lock-color](https://github.com/Raymo111/i3lock-color)
* 壁纸管理: [variety](https://peterlevi.com/variety/)(需要feh支持)
#### 简单配置
记录第一次用linux发行版本Kali对其桌面美化的尝试
* 参考[视频](https://www.bilibili.com/video/BV1KR4y127dR/?spm_id_from=333.1007.top_right_bar_window_history.content.click&vd_source=8836eda798f42e634172036484104534)
* [Gnome-extension](https://extensions.gnome.org/)初次登陆需要安装[Mozilla Addons](https://addons.mozilla.org/en-US/firefox/addon/gnome-shell-integration/)扩展到Firefox和本地连接器`$ sudo apt install chrome-gnome-shell`温馨提示：基本上默认都带，无需安装
  - 插件:
    1. User Themes : 从user目录中加载shell主题
    2. Dash to Dock : 一个在桌面底部的Dock
    3. Desktop Cube : 3D切换workspace
    4. Aylur's Widget : 状态栏
  - 配置插件置只须点击插件开关旁边的button，也可通过本地(自带)扩展进行配置
* Cursor theme: [Nordzy-cursors](https://github.com/alvatip/Nordzy-cursors)
* Icon theme: [Nordzy-icon](https://github.com/alvatip/Nordzy-icon)
* User theme: [WhiteSur-gtk-theme](https://github.com/vinceliuice/WhiteSur-gtk-theme)
* FAQ:
  - Auto move windows 会影响正常的workspace使用，具体导致其dynamic workspace失效，[issue](https://gitlab.gnome.org/GNOME/gnome-shell/-/issues/290) is here!
  - Icon theme的安装脚本好像有些问题，会出现以下报错。`sed:无法读取 /home/hunter/.local/share/icons/Nordzy-dark/actions/32/dialog-selectors.svg`
    这边建议直接通过[压缩包](https://www.pling.com/s/Gnome/p/1686927)解压，手动安装
### 代理
{% note success %}
想要富先修路 - 代理
{% endnote %}
配置Arch的代理非常重要，详细内容请见{% post_link 代理 %}
{% note danger %}
配置代理后，一定要换源
{% endnote %}
### 改键
{% note info %}
一切以简单实用为前提，向可移植性靠拢!
{% endnote %}
#### 前置知识
![Linux键盘工作原理](/images/computer/os/arch/kbd.png)
xbindkeys: 实现多个组合键执行一个命令，例如ctrl+i+j = `echo hello world`
xdotool: 模拟键位按压，例如 j = k
interception-tools: 基于evdev协议实现改键(不光能改键)
查看按下xorg的键位码: `xev`
查看xorg映射键位码: `xmodmap -pke | less`
查看xorg修饰键位: `xmodmap`
恢复xorg键位初始设置: `setxkbmap`
查看按下内核的键位码: `sudo showkey # 不加sudo就只能在TTY下执行`
#### 改键方式
##### 方式一
caps -> ctrl
通过`~/.Xmodmap`文件实现改键

```
clear lock
clear control
add lock = Control_L
add control = Caps_Lock
keycode 37 = Caps_Lock
keycode 66 = Control_L
```
使用该文件进行改键需要注意不单单要更改键码与键符的映射，还需要更改修饰键位
xorg键位映射码如keycode n = keysym1 keysym2 keysym3 keysym4
其中原理如下
keysym1 = n
keysym2 = shift + n
keysym3 = mode_switch + n
keysym4 = mode_switch + shift + n
因此先要实现ctrl+space=esc则需要修改mode_switch为ctrl，但是ctrl就会失效
因此对于vim用户，不建议使用这种改键方式
只需要添加`~/.Xomdmap`文件并在其中添加以下内容即可
##### 方式二
caps -> ctrl + esc
单独按下caps时为caps，按下caps+其它键位时为ctrl
[参考文档](https://zhuanlan.zhihu.com/p/404819427)
用yay安装`interception-dual-function-keys`(其他发行版本参考仓库地址编译)
添加/etc/interception/caps2ctrlesc.yaml文件，并在其中添加以下内容
```
TIMING:
    TAP_MILLISEC: 200 # 点击小于200毫秒判定为esc
    DOUBLE_TAP_MILLISEC: 150 # 可选配置

MAPPINGS:
    - KEY: KEY_CAPSLOCK
      TAP: KEY_ESC
      HOLD: KEY_LEFTCTRL
```
添加/etc/interception/udevmon.d/dual-function-keys.yaml，并在其中添加以下内容
```
- JOB: "intercept -g $DEVNODE | dual-function-keys -c /etc/interception/capslock2ctrlesc.yaml | uinput -d $DEVNODE"
  DEVICE:
    NAME: "AT Translated Set 2 keyboard" # 任务作用的设备
- JOB: "intercept -g $DEVNODE | dual-function-keys -c /etc/interception/capslock2ctrlesc.yaml | uinput -d $DEVNODE"
  DEVICE:
    NAME: "FILCO Bluetooth Keyboard"
```
如果想把改键配置放在其他路径, 记得修改上面配置相同的路径, 几个键盘设备就对应配置几个JOB
NAME参数的设备名称通过这个命令确定`sudo uinput -p -d /dev/input/by-id/X`, X是路径下类似keyboard名称的设备, 如果这个路径下没有, 则需要在`/dev/input`目录下慢慢找了
配置写好后启动进程即可`systemctl enable --now udevmon.service`
### 应用软件合集
{% note success %}
手巧不如家什妙，磨刀不误砍柴工 - 应用软件
{% endnote %}
{% note info %}
本人是开源软件爱好者，并不喜欢闭源软件，尤其是连接网络的闭源软件
针对闭源软件的缺点本人在此不想过多赘述，懂的都懂
本人在此也强烈建议使用开源软件避免不必要的麻烦
{% endnote %}
对于下面软件的使用本人的使用建议是stfw,rtfm
#### 硬件控制
* 声音: alsa-utils(Alsamixer, Amixer(图形界面))
* 亮度: light
* 蓝牙: bluetoothmanager
* 网络: [networkmanager](https://wiki.archlinuxcn.org/wiki/NetworkManager)
* 电源: [tlp](https://linrunner.de/tlp/usage/index.html#start)
{% note success %}
亲测，用了2年的联想小新在tlp的battery模式下使用时长为4小时
{% endnote %}
#### 常用软件
* 终端模拟器: [alacritty](https://alacritty.org/config-alacritty.html)
* 修图: gimp
* 图片查看器: feh
* 输入法: [fcitx5](https://wiki.archlinuxcn.org/wiki/Fcitx5)
* 浏览器: edge/firefox
* 截屏: flameshot
* AI: copilot(edge)
* Shell: zsh + ohmyzsh
* 命令行辅助器: man + tldr
* 文件查看器: less
* 文件管理: ranger
* 文本编辑器: vi/vim
* 虚拟化与容器: vmware + docker
* 版本控制: git
* 密码管理器: pass
## 使用Arch
本人的工作流比较简单
开发: 基于docker，搭建开发环境，避免开发工具，库冲突等
工作: 对于工作文档等内容，在windows上解决
娱乐: 本人玩红警，在windows上解决
## 尾声
看到这里，关于本人的Arch从安装到使用基本上都阐述完成
我的故事尚未结束，但是看到这篇博客的arch新人与arch的故事才刚刚开始...
## FAQ
### copilot
copilot是bing的插件，配置好代理然后用不了copilot估计是bing的地区错误
改一下bing的设置，将bing的地区改成国外即可
若改完还是不行，有一定概率是dns污染所导致
`sudo vim /etc/resolv.conf`添加`namserver 8.8.8.8`即可(好像是google的dns解析器)
### 未安装netmanager
重新插上引导盘，进入安装的Arch，重新安装一下networkmanager即可
本人当时记得安装了，所以具体操作流程本人也不知道
### grub相关
* 找不到windows是正常的，进系统后更新一下grub`sudo grub-mkconfig -o /boot/grub/grub.cfg`重启即可看到
* 找不到Arch这种情况本人也没遇到过，不知道咋办，不过我猜大概率是配置grub的是很配置错误，和找不到networkmanager的解决办法应该差不多，插上引导盘重新设置即可
* 找不到grub这种情况本人也没遇到过，不知道咋办，不过我猜测大概率是安装grub服务的位置安装错误，插上引导盘，确保安装服务位置正确，重新安装grub服务即可
### NVIDIA
* 对于定制内核，比如我的zen内核，就需要阅读[这个](https://wiki.archlinuxcn.org/wiki/NVIDIA#%E5%AE%9A%E5%88%B6%E5%86%85%E6%A0%B8)
* 对于具有intel和NVIDIA双显卡的笔记本需要阅读[这个](https://wiki.archlinuxcn.org/wiki/NVIDIA_Optimus)
