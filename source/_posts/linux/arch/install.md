---
title: 安装arch
categories:
- linux
- arch
tags:
- arch
category_bar: true
index_img: /images/linux/arch/archlogo.jpeg
---
简介: 记录本人安装arch的相关内容
<!-- more -->
## 安装前
{% note danger %}
文档看得越仔细,安装越顺利,反之亦然
{% endnote %}
### 装前须知(废话)
{% note info %}
安装arch用的引导系统没有引导
{% endnote %}
装系统总共分三步:
* ~~把冰箱门打开~~ 进入安装引导系统
* ~~把大象放到冰箱里~~ 装系统
* ~~关上冰箱门~~ 重启进入新系统
好吧,上面说了等于没说,hhh
### 准备引导动盘
下载arch镜像,准备启动引导盘(网上教程多的是,这里不赘述)
这里使用的烧录软件是[rufus](https://rufus.ie/en/#)
### 配置引导系统
{% note info %}
手机可以访问arch官网,推荐一边看手机，一边进行操作
{% endnote %}
本质上就是通过引导系统从网上下载arch内核装到磁盘中
这里涉及网络配置,镜像地址修改,磁盘分区,挂载等(详细内容见官网教程)
* 连不上网可能是因为网卡被[rfkill](https://wiki.archlinux.org/title/Network_configuration/Wireless#Rfkill_caveat)
* 创建分区建议使用cfdisk
* 想要创建分区时发现没有发现空白分区是因为空白分区并不是块设备
* 已经有EFI分区的话，就不用再创建EFI分区
* 针对不同分区，格式化的方式不同
* 针对不同分区，挂载的方式不同,挂载有顺序要求
* 挂在到/mnt目录是因为/mnt目录是你要安装的新系统的根目录
* 由于arch装系统是从网上下载系统,因此需要配置引导系统的镜像地址,可以使用[arch镜像源生成网站](https://archlinux.org/mirrorlist/)
## 安装
进入到这里,就正式开始安装系统了
此刻,不需要安装太多的软件,只需要安装些必要的软件即可
剩下的之后{% post_link 配置arch %}时再装
* 盘点安装所需包(不是全都完全必要,但是当时我装了这些内容)
  ```
  linux-zen linux-zen-headers # 内核与其头文件
  base base-devel # 最小软件包组base与其扩展
  linux-firmware # 固件包
  intel-ucode # 用于处理intel处理器微码更新的软件包
  grub # 启动引导程序
  os-prober # 用于探测磁盘中已存在的操作系统，为grub创建菜单项
  efibootmgr # 用于管理UEFI启动项的命令行工具
  ntfs-3g # 用于读写windows类别的ntfs格式的分区
  dkms # 用于动态内核模块支持系统，主要用于在内核升级时自动编译和安装第三方内核模块，确保与新系统兼容被nvidia-dkms依赖
  nvidia-dkms # 显卡
  xorg # x11具体实现
  vim man # 编辑器与手册
  networkmanager # 网络管理
  ```
* 没有安装xorg包之前就关机会出现关机后电源灯还亮着且没真正关机这种情况(具体原因不到,反正装完xorg后就好使了...)
* 不安装驱动程序是因为通用驱动一般包含在内核内
* 注意，内核包含的驱动程序是通用驱动，但是对于如英伟达这样的就需要[特别安装驱动](#NVIDIA)
### 安装后
经过一段时间的等待,上面安装的内容已经安装完了,需要进行简单的配置
大致就是本地化,时间,网络,用户,引导等相关配置
以下是相关配置操作
* 零碎操作备忘
  ```
  genfstab -U /mnt >> /mnt/etc/fstab # 自动挂载配置文件
  ln -sf /usr/share/zoneinfo/Asia/Shanghai /etc/localtime #设置时区
  # 删除/etc/locale.gen中en_US.UTF-8 和 zh_CN.UTF-8 UTF-8 前的# locale-gen # 生成locale
  # 编辑/etc/locale.conf 添加LANG=en_US.UTF-8
  # 编辑/etc/hostname 添加主机名称
  passwd root # 设置root的密码
  useradd -m -G wheel [username] # 创建普通用户将这个用户加入到wheel组中
  passwd [username] # 设置username的密码
  # 编辑sudoers赋予用户root的sudo权限，删除%wheel ALL=(ALL:ALL) ALL前的#
  # 用systemctl 启用蓝牙和网络服务
  # 编辑/etc/default/grub，去掉GRUB_DISABLE_OS_PROBER=false前的#
  # 安装grub服务
  # 其中参数是可以改变的，详细内容看官方文档，参数错误就会造成grub安装位置错误
  grub-install --target=x86_64-efi --efi-directory=/efi --bootloader-id=grub
  grub-mkconfig -o /boot/grub/grub.cfg # 更新grub引导
  ```
* 设置时区之后使用[NTP](https://en.wikipedia.org/wiki/Network_Time_Protocol)客户端使系统时间同步网络时间
* [语言本地化](https://wiki.archlinuxcn.org/wiki/%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87%E6%9C%AC%E5%9C%B0%E5%8C%96)建议遵循建议在.xprofile中进行设置
### 安装结束
到这步,可以重启Arch，拔掉引导盘
如果一切顺利的话，电脑重启会进入grub，在那里，可以找Arch系统和之前的系统。
这里安装Arch可以说是基本结束了，剩下的就开始配置Arch了
## FAQ
### 没装NetworkManager
通过引导盘把NetworkManager装里就行,不需要全重装
### NVIDIA
* 对于定制内核，比如本人使用了zen内核，就需要阅读[这个](https://wiki.archlinuxcn.org/wiki/NVIDIA#%E5%AE%9A%E5%88%B6%E5%86%85%E6%A0%B8)
* 对于具有intel和NVIDIA双显卡的笔记本需要阅读[这个](https://wiki.archlinuxcn.org/wiki/NVIDIA_Optimus)
### grub找不到windows
找不到windows是正常的，进系统后更新一下grub`sudo grub-mkconfig -o /boot/grub/grub.cfg`重启即可看到
