---
title: Shell脚本
categories:
- 开发
- 语言
- 脚本
category_bar: true
---

如何在shell脚本中使用sudo
方法1: 在sudoers中添加如下内容
`username ALL=(ALL) NOPASSWD: /path/to/your/script`
这使程序当以sudo的方式执行时不用输入密码
方法2: 对脚本进行操作
`echo yourpassword | sudo -S tlp-stat -b`
这个方式简单粗暴，但是有安全问题

