---
title: vim
categories:
- Linux
tags:
- Linux
sticky: 90
category_bar: true
---
简介: 记录个人使用vim的心得
<!-- more -->
## 当前效果展示
![neovim效果](/images/linux/arch/codes.png)
## 相关资源
{% note info %}
容易找到的就不放置在此
{% endnote %}
* [编辑器圣战](https://www.bilibili.com/video/BV1XA4y1o7Lw/?spm_id_from=333.337.search-card.all.click&vd_source=8836eda798f42e634172036484104534)
* [vimawesome(插件市场)](https://vimawesome.com/)
* [vim book](https://www.truth.sk/vim/vimbook-OPL.pdf)
* [vim cookbook](http://www.oualline.com/vim-cook.html)
* [Seven habits of effective text editing](https://www.moolenaar.net/habits.html)
* [VIM User Manual](http://www.eandem.co.uk/mrw/vim/usr_doc/index.html)
## 个人配置
* [vim配置]()
* [neovim配置]()
## 插件合集
### UI
* bufferline: 缓冲区栏
* dashboard: neovim封面
* lualine: 状态栏
* mini.indentscope: 缩进线(动效显示)
* noice: Tui命令输入框
  - nui: 图形库
* nvim-notify: 消息报告
* nvim-tree: 目录树
  - nvim-web-devicons: 图标支持
* neovide: 严格来说不算插件，其实是neovim的GUI版重构，实现光标移动，窗口打开等各种酷炫效果
* tokyonight: neovim主题
### 编码
#### 补全
* nvim-cmp: 补全
  - hrsh7th/cmp-nvim-lsp: lsp补全
  - hrsh7th/cmp-buffer: buffer补全
  - hrsh7th/cmp-path: 文件路径补全
  - hrsh7th/cmp-cmdline: 命令补全
  - tzachar/cmp-tabnine: ai补全
  - f3fora/cmp-spell: 拼写补全
* mason: lsp,dap,format管理
* mason-lspconfig: mason适配lsp
* nvim-lspconfig: nvim适配lsp
#### 高亮
* nvim-treesitter: 语言高亮
* nvim-treesitter-textobjects: 使选择文本更加智能
* nvim-treesitter-context: 显示嵌套头部内容
* vim-illuminate: 高亮当前文件中的所有和 “当前光标所处单词” 一样的单词
### 编辑
* which-key: 键位提示
* vim-visual-multi: 多行编辑(影分身)
* flash: 光标快速移动(闪现)
* mini.pairs: 自动配对
* mini.surround: 快速添加括号
* nvim-spectre: 全局替换
### 工具
* lazy.vim: 插件管理器(也管理插件启动)
* mini.bufremove: 关闭缓冲区
* persistence: 恢复上次开启的buffer
* vim-startuptime: 计算neovim启动时间
## FAQ
### 选择
* vim vs neovim vs neovide
  vim：编辑器,neovim：vim的升级版,neovide：GUI版vim
  个人认为,neovim比vim能更现代化一些,有用lua写的lazy.nvim等现代化插件
  但vim则更加稳定,而二者本质区别并不是很大,且二者插件体系并无太大差别
  只是配置文件有些许差别,其中neovim能更加结构化
  追求稳定传统可以选择vim,追求新颖现代化可以选择neovim
  最终我选择使用neovide,因为更加酷炫hhh
* "vim" vs "vscode" vs "VisualStdio"
  本人率先接触到了vim,之后尝试了一下vscode
  vscode和vim同样作为编辑器,显然vscode更加容易配置
  不过vscode可配置性不如vim,开箱即用以及健壮性不如Visualstdio
  个人感觉vscode并不适合用来写C/C++
* "编辑器之神" vs "神之编辑器"
  本人选择使用vim,纯纯是因为先接触到了vim,对于emacs并不了解
  但是仍旧在shell中感受到了ctrl+n/p/b/f这种移动方式的强大
### 配置问题合集
#### neovide
* [neovide中lualine和bufferline不贴边](https://github.com/neovide/neovide/issues/2646)
* neovide如何实现英文一种字体，中文一种字体，在neovide的官方文档中写到`The basic format is Primary\ Font,Fallback\ Font\ 1,Fallback\ Font\ 2:option1:option2:option3, while you can have as many fallback fonts as you want (even 0) and as many options as you want (also even 0).`neovide中支持一种主力字体，还有多种备用字体，可将中文字体设置成备用字体
