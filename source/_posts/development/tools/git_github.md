# Github
## 简介
托管平台
## 使用
git 注册
git config --global user.name "hunter"
git config --global user.email "284050500@qq.com"
* SSH Key
  ```
  ssh-keygen -t rsa -C "邮箱"
  cd ~/.ssh
  cat id_rsa.pub
  ```
复制cat的运行结果到Github中的setting的ssh选项中即可通过SSH连上Github  

## 高级搜索
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
