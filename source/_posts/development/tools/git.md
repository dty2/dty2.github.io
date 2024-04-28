---
title: git
categories:
- 开发
- 开发工具
tags:
- git
category_bar: true
---
## git
开始使用前先进行git注册
git config --global user.name "hunter"
git config --global user.email "284050500@qq.com"
* SSH Key
  ```
  ssh-keygen -t rsa -C "邮箱"
  cd ~/.ssh
  cat id_rsa.pub
  ```
复制cat的运行结果到Github中的setting的ssh选项中即可通过SSH连上Github  
## .gitignore
## FAQ
已被追踪的文件/目录即使后来添加到.gitignore中也不会生效
若想要生效，则需要删除缓存
`git rm --cache <file> # 针对文件`
`git rm -r --cache <directory> # 针对目录`
## github使用
### 高级搜索
in:name: Qt
in:readme: Qt 音频
in:description: Qt 音频
in:language:c++
in:stars:>1000
in:pushed:>2021-09-01
in:focks:>1000
## FAQ
1. github仓库传图片不显示的问题
[解决方案](https://blog.csdn.net/admin_maxin/article/details/134498227)
大致就是DNS污染的问题，换一个DNS解析器就行了
`sudo vim /etc/resolv.conf`添加`namserver 8.8.8.8`即可(好像是google的dns解析器)
