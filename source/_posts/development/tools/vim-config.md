---
title: 配置vim
categories:
- 开发
- 开发工具
tags:
- vim日记
sticky: 80
category_bar: true
index_img: /images/3.png
---
# 前情提要
- 花里胡哨毫无意义，实用才是根本
- 不保证以下内容全部正确，在使用代码块时最好懂其作用
# 配置简介
配置vim简单来说分为两步，分别是编写.vimrc文件和装插件
(没有先后顺序，是交替完善的过程，推荐第一次配置vim先编写.vimrc)
我的.vimrc文件在[这里]()
# 配置解读
## 基础配置
以下内容对应我个人.vimrc
- `let g:plug_url_format = 'git@github.com:%s.git'`
使用ssh安装插件，能避免一些众所周知的问题
- `syntax on` 语法高亮
- `filetype` 文件识别，一般和语法高亮一起使用
- `set number` 设置行号
- `set relativenumbe` 设置相对行号
- `set wildmenu` 命令补全
- `set showcmd` 显示命令
- `set title` 在顶部显示标题
- `set wrap` 自动换行
- `set hlsearch` 高亮搜索
- `set scrolloff=5` 光标距顶/底5行
- `set backspace=indent,eol,start` backspace 能删除缩进，行尾/首空格
- `set laststatus=2` 状态栏2行
- `set cursorline` 高亮当前行
- `set cursorcolumn` 高亮当前列
- `colorscheme gruvbox` 主题为gruvbox
- `set smartindent` 智能缩进
- `set expandtab` tab 转换成space
- `set tabstop=2` tab占4个空格
- `set softtabstop=2` 插入模式tab占4个空格
- `set shiftwidth=2` 智能缩进为4个空格
- `set clipboard=unnamedplus`
复制到系统剪贴板，若支持"+clipboard"，就能跨终端粘贴vim中的内容
- 定位光标到上次退出时编辑的位置
```
augrop resCur
  autocmd!
  autocmd BufReadPost * call setpos(".", getpos("'\'"))
augroup END
```
- Wsl中，复制内容到Windows剪贴板
```
let s:clip = '/mnt/c/Windows/System32/clip.exe'
if executable(s:clip)
    augroup WSLYank
        autocmd!
        autocmd TextYankPost * if v:event.operator ==# 'y' | call system(s:clip, @0) | endif
    augroup END
endif
```

---

## 操作配置
- `let mapleader= ' '` 空格作为leader键位
- `nnoremap <leader>w :w<CR>` leader + w 保存
- `nnoremap <leader>q :q<CR>` leader + q 退出
- `nnoremap <leader>t :vertical terminal<CR>` leader + t 竖开终端
- `nnoremap <leader>ti :terminal<CR>` leader + ti 横开终端
- `nnoremap <leader>] :v<C-w><C-]><CR>` leader + ] 忘了...和ctags有关
- `nnoremap <leader>]i :v<C-]><CR>` leader + ]i  忘了...和ctags有关
- `nnoremap <leader><CR> :nohlsearch<CR>` leader + enter 取消搜索高亮
- `nnoremap <c-f> <c-f>zz` 向下翻半页且置中
- `nnoremap <c-b> <c-b>zz` 向上翻半页且置中
- `nnoremap n nzz` 查找下一个且置中
- `nnoremap N Nzz` 查找上一个且置中
- `nnoremap ; :` ;开启命令
- `inoremap jj <Esc>` jj等于esc
- `nnoremap R :source $MYVIMRC<CR>` R刷新配置
- `nnoremap sl :set splitright<CR>:vsplit<CR>` 竖开分屏
- `nnoremap sj :set splitbelow<CR>:split<CR>` 横开分屏
- `nnoremap <leader>sh :vertical resize -5<CR>` 分屏横向大小减少5px
- `nnoremap <leader>sl :vertical resize +5<CR>` 分屏横向大小增加5px
- `nnoremap <leader>sj :res -5<CR>` 分屏大小竖向减少5px
- `nnoremap <leader>sk :res +5<CR>` 分屏大小竖向增加5px
- `nnoremap <c-h> <c-w>h` ctrl + h 光标移动到左分屏
- `nnoremap <c-l> <c-w>l` ctrl + l 光标移动到右分屏  

# 装插件
给vim装插件犹如给手机加翅膀，让其开启"飞行模式"
使用vim不用插件，几乎等同于吃饭不吃菜，只吃主食
以下内容中的依赖使用debain的包管理器安装
对于arch用户，若非需要特殊插件，否则直接用pacman获取即可

## vim-plug
  * 介绍: 插件管理器
  * 安装vim-plug
    ```
    cd ~
    git clone git@github.com:junegunn/vim-plugins.git
    mkdir -p ~/.vim/autoload/
    cp ~/vim-plug/plug.vim .vim/autoload/plug.vim
    ```
  * 配置vimrc
    ```
    let g:plug_url_format = 'git@github.com:%s.git' "使用ssh下载插件
    call plug#begin('~/.vim/plugged')
    Plug 'xxx-xxx-xxx' "替换
    call plug#end()
    ```
  * 使用
    * 安装插件
      在两个call中间添加`Plug 'xxx-xxx-xxx'`
      更新配置文件`source ~/.vimrc`
      插件安装`PlugInstall`
    * 删除插件
      在两个call中间删除`Plug 'xxx-xxx-xxx'`
      更新配置文件`source ~/.vimrc`
      插件安装`PlugClean`

---

## Nerdtree
  - 介绍: 文件树
  - 配置:
    - `nnoremap <leader>n :NERDTreeToggle<CR>` 开关
    - `let NERDTreeQuitOnOpen = 1` 打开文件自动关闭
    - `let NERDTreeShowHidden = 1` 显示隐藏文件

---

## Airline & Airline Themes
  - 介绍: 状态栏 + 美化
  - 安装依赖: powerline: `sudo apt-get install fonts-powerline` (也可不下载)
  - 配置:
    - `let g:airline#extensions#tabline#enabled = 1` 打开顶栏
    - `let g:airline_powerline_fonts = 1` 打开powerline字体效果
    - 添加airline主题
      ```
      if !exists('g:airline_symbols')
      let g:airline_symbols = {}
      let g:airline_theme="gruvbox"
      endif
      ```
    - 不用下载powerline字体，请在vimrc文件中添加如下内容(也能获得类似效果)
      ```
      if !exists('g:airline_symbols')
      let g:airline_symbols = {}
      let g:airline_left_sep = ''
      let g:airline_right_sep = ''
      let g:airline_symbols.branch = ''
      let g:airline_symbols.readonly = ''
      let g:airline_symbols.dirty='⚡'
      endif
      ```
  - [帮助1](https://www.youtube.com/watch?v=-r6Sj70Ziws)
  - [帮助2](https://tuckerchapman.com/posts/getting-started-vim-airline/)
  - FAQ:
    - 获取字体命令不生效则手动上powerline仓库[去安装](https://github.com/powerline/fonts)
    - 如果你在使用wsl2的话，需要你检查你的windows是否有字体集，如果没有则需要在windows上安装相应字体集，并在终端模拟器上进行配置

---

## Tagbar
  - 介绍: 标签栏
  - 安装依赖: ctags: 执行`sudo install apt exuberant-ctags`下载ctags
  - 配置:
    - `nnoremap <leader>m :TagbarToggle<CR>` 开关
    - `let g:tagbar_width = 20` 设置tagbar的宽度为20
    - `let g:tagbar_autofocus = 1` 打开后自动将光标置入其中
    - `let g:tagbar_autoclose = 1` 选择确定后自动关闭

---

## vim-Surround
  - 介绍: 围绕指定内容增加包裹字符
  - FAQ: [What does the "y" stand for in "ysiw"?](https://github.com/tpope/vim-surround/issues/128)

---

## Gruvbox
  - 介绍: vim主题

---

## visual-multi
  - 介绍: 多光标
  - 配置:
    - `let g:vM_theme = 'iceblue'` 主题为iceblue
    - `let g:EasyMotion_leader_key = '\'` leaderkey设为\
    - 映射
      ```
      let g:vM_maps = {}`
      let g:vM_maps['Add Cursor Up'] = '<C-k>'` ctrl + k 向上增加光标
      let g:vM_maps['Add Cursor Down'] = '<C-j>'` ctrl + j 向下增加光标
      ```

---

## Easymotion
  - 介绍: 闪现，快速跳转
  - 配置: `map <Leader><leader> <Plug>(easymotion-s)` leader + leader 开启闪现

---

## Coc-nvim
  - 介绍: 自动补全平台
  - 安装依赖: nodejs
    这里展示通过nvm安装，安装nvm`curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/master/install.sh | bash`
    ps: 重启终端使其生效
    安装nodejs`nvm install 16.18.0`
    ps: 注意版本！！！官方的readme说大于14.14.0是不行的，进入vim就报错的话，报错里会有一行说版本小于16.18.0
  - 使用
  - 安装插件中的插件: `CocInstall xxx`
    - tabnine: 智能补全
    - pairs: 配对补全
    - html: html补全
    - css: css补全
    - tsserver: javascript补全
    - clangd: C++补全
    - json: json补全
    - sh: shell补全
    - yaml: yaml补全
    - highlight: 语法高亮
  - 配置:
    - `:CocConfig` 在打开文件中添加
        ```
        {
          "coc.source.tabnine.enable":true
        }
        ```
    - `:CocCommand tabnine.openConfig` 在打开文件中添加 `"ignore_all_lsp":true`
      (该选项意思是屏蔽其他lsp插件，若希望补全协同工作，请不要设置成true)
  - [帮助](https://github.com/neoclide/coc-tabnine/issues/24)
  - FAQ:
    - 当执行`CocCommand tabnine.openConfig`的时候，没有任何事发生
      这可能是由于相应的配置文件不存在所导致的。可以查看在.config目录下存不存在Tabnine文件。如果不存在则证明配置文件缺失。导致配置文件缺失可能是由于`./config/coc/extensions/coc-tabnine-data/binaries/4.5.19/x86_64-unknown-linux-musl/Tabnine`文件没有执行权限所导致的。你需要给其增加执行权限`chmod +x Tabnine`。接着执行`:CocCommand tabnine.updateTabNine`, 在这时你需要按压其他键位，你会看到在状态栏中有个进度条，让其到达100%即可。最后重新下载`:CocInstall coc-tabnine` 并尝试 `:CocCommand tabnine.openConfig` 即可
    - 在使用kali的时候，由于ssh并未设置相应的端口，导致下载插件失败，只需将端口由10808更改成10810即可

# 其他
## 构建vim
很多时候，原生vim满足不了我们的需求  
举个最简单的例子，我们需要vim支持“+clipboard”，但是从pacman或者apt等包管理器安装的vim并不支持clipboard  
这时候，就需要我们删除掉我的用包管理器安装的vim，手动去github上clone下来vim的源码，手动去在本地编译它
以下是本地构建vim的过程记录  
- 配置参数  
``` bash
# 需要什么功能，自行添加参数
./configure \
--with-features=huge \
--enable-fontset \
--enable-python3interp \
--with-python3-command=python3
```
- 查询cpu数量`cat /proc/cpuinfo | less` (ps:查看逻辑cpu数量，下面编译的时候使用-jx来进行并行编译)  
- `sudo make -j8` + `sudo make install`  (ps:注意看vim官方编译教程，上面要求命令执行在固定位置)
- 查看vim版本`vim --version`,看是否有"+clipboard"，如果有则构建成功

PS：不建议Arch用户这么做，这可能会导致包冲突等问题  
目前，在Arch上如何解决vim的剪贴板问题，我也不知道  

- 教训: 最初我在重新编译vim的时候，是在一篇博客的指导下完成的。最初我在查阅该博客的时候，我并未打算让vim支持lua等内容。可是当时我在照着那个博客配置参数的时候，并未注意到`--enable-fail-if-missing`这个参数，这个参数的意思是只有每个参数都配置成功才整体配置成功。但是由于我未安装lua等相关依赖，又照搬人家博客中的配置参数(含有与lua有关的参数)，最终导致我怎么尝试也不好使。最终无奈，特意查询了每个参数的含义后我才明白是怎么回事。
FAQ:
- no terminal library found
  checking for tgetent()... configure: error: NOT FOUND!
        You need to install a terminal library; for example ncurses.
        On Linux that would be the libncurses-dev package.
        Or specify the name of the library with --with-tlib.
  make[1]: *** [Makefile:1972: config] Error 1
  make[1]: Leaving directory '/home/hunter/vim/src'
  make: *** [Makefile:2000: reconfig] Error 2
  这是缺少一个依赖库，安装上就行了`sudo apt-get install libncurses-dev`

- 构建失败删除vim删不了
  进如`/user/local/bin`中删除vim，之后再删除之前构建vim的vim目录，别忘了使用sudo

## 帮助文档
[vim webpage address](https://www.vim.org/)
[vim github address](https://github.com/vim/vim)
[Vim book](https://www.truth.sk/vim/vimbook-OPL.pdf)
[Vim Cookbook](http://www.oualline.com/vim-cook.html)
[Seven habits of effective text editing](https://www.moolenaar.net/habits.html)
[vim User Manual](http://www.eandem.co.uk/mrw/vim/usr_doc/index.html)
[vim awesome](https://vimawesome.com/)
