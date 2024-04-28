---
title: vim
categories:
- 开发
- 开发工具
category_bar: true
---
简介: vim与它的配置
<!-- more -->
{% note info %}
花里胡哨无用，实用至上
{% endnote %}
## 帮助
* [vim官网](https://www.vim.org/)
* [vim github](https://github.com/vim/vim)
* [Vim book](https://www.truth.sk/vim/vimbook-OPL.pdf)
* [Vim Cookbook](http://www.oualline.com/vim-cook.html)
* [Seven habits of effective text editing](https://www.moolenaar.net/habits.html)
* [vim User Manual](http://www.eandem.co.uk/mrw/vim/usr_doc/index.html)
* [vim awesome](https://vimawesome.com/) ps: 用来找插件
## 简介
vim，编辑器之神
## 配置文件
我的.vimrc文件在[这里](https://github.com/dty2/dotfile/blob/main/vim/.vimrc)
### 基础配置
- `let g:plug_url_format = 'git@github.com:%s.git'` 使用ssh安装插件
- `syntax on` 语法高亮
- `filetype` 文件识别，一般和语法高亮一起使用
- `set number` 行号
- `set relativenumbe` 相对行号
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
- `set expandtab` tab转换成space
- `set tabstop=2` tab占4个空格
- `set softtabstop=2` 插入模式tab占4个空格
- `set shiftwidth=2` 智能缩进为4个空格
- 复制到系统剪贴板，若支持"+clipboard"，就能跨终端粘贴vim中的内容
  `set clipboard=unnamedplus`
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
### 键位映射
{% note info %}
个人习惯，仅供参考
{% endnote %}
- `let mapleader= ' '` 空格为leader键位
- `nnoremap <leader>w :w<CR>` leader+w保存
- `nnoremap <leader>q :q<CR>` leader+q退出
- `nnoremap <leader>t :vertical terminal<CR>` leader+t竖开终端
- `nnoremap <leader><CR> :nohlsearch<CR>` leader+enter关闭搜索高亮
- `nnoremap n nzz` 查找下个匹配项且置中
- `nnoremap N Nzz` 查找上个匹配项且置中
- `nnoremap ; :` 一键开启底部命令行
- `nnoremap R :source $MYVIMRC<CR>` 刷新配置
- `nnoremap sl :set splitright<CR>:vsplit<CR>` 竖开分屏
- `nnoremap <c-h> <c-w>h` ctrl + h 光标移动到左分屏
- `nnoremap <c-l> <c-w>l` ctrl + l 光标移动到右分屏  
- `nnoremap <leader>] :v<C-w><C-]><CR>` leader+]跳转到对应函数定义
- `nnoremap <leader>]i :v<C-]><CR>` leader + ]i  忘了...和ctags有关
## 插件
* vim-plug 插件管理器
* Nerdtree 目录树
  - `nnoremap <leader>n :NERDTreeToggle<CR>` 开关
  - `let NERDTreeQuitOnOpen = 1` 打开文件自动关闭
  - `let NERDTreeShowHidden = 1` 显示隐藏文件
* Airline & Airline Themes
  - `let g:airline#extensions#tabline#enabled = 1` 打开顶栏
  - `let g:airline_powerline_fonts = 1` 打开powerline字体效果
  - 设置主题
    ```
    if !exists('g:airline_symbols')
    let g:airline_symbols = {}
    let g:airline_theme="gruvbox"
    endif
    ```
  - 检查当前系统是否有[powerline字体](https://github.com/powerline/fonts)字体支持
  - [帮助1](https://www.youtube.com/watch?v=-r6Sj70Ziws)
  - [帮助2](https://tuckerchapman.com/posts/getting-started-vim-airline/)
* Tagbar 标签栏
  - 需要提前安装ctags
  - `nnoremap <leader>m :TagbarToggle<CR>` 开关
  - `let g:tagbar_width = 20` 设置tagbar的宽度为20
  - `let g:tagbar_autofocus = 1` 打开后自动将光标置入其中
  - `let g:tagbar_autoclose = 1` 选择确定后自动关闭
* vim-Surround 围绕指定内容增加包裹字符
  - [What does the "y" stand for in "ysiw"?](https://github.com/tpope/vim-surround/issues/128)
* Gruvbox vim主题
* visual-multi 多光标
  - `let g:vM_theme = 'iceblue'` 主题为iceblue
  - `let g:EasyMotion_leader_key = '\'` leaderkey设为\
  - 设置映射
    ```
    let g:vM_maps = {}`
    let g:vM_maps['Add Cursor Up'] = '<C-k>'` ctrl + k 向上增加光标
    let g:vM_maps['Add Cursor Down'] = '<C-j>'` ctrl + j 向下增加光标
    ```
* Easymotion 快速跳转(我更喜欢叫“闪现”)
  - `map <Leader><leader> <Plug>(easymotion-s)` leader+leader开启闪现
* Coc-nvim 补全插件平台
  - tabnine: 智能补全
  - pairs: 配对补全
  - html: html补全
  - css: css补全
  - tsserver: javascript补全
  - clangd: C++补全
  - cmake: cmake补全
  - json: json补全
  - sh: shell补全
  - yaml: yaml补全
  - highlight: 语法高亮
  - `:CocCommand tabnine.openConfig` 在打开文件中添加 `"ignore_all_lsp":true`
    该选项意思是屏蔽其他lsp插件，若希望补全协同工作，请不要设置成true
  - [帮助](https://github.com/neoclide/coc-tabnine/issues/24)
## 其他
### 构建vim
有时候，原生vim满足不了我们的需求  
比如我们需要vim支持“+clipboard”，但是从pacman或者apt等包管理器安装的vim并不支持clipboard  
- 配置参数
```
# 需要什么功能，自行添加参数
./configure \
--with-features=huge \
--enable-fontset \
--enable-python3interp \
--with-python3-command=python3
```
- 注意看vim官方编译教程，上面要求命令执行在固定位置)
- 查看vim版本`vim --version`,看是否有"+clipboard"，如果有则构建成功
- 记一个教训: 最初在重新编译vim的时候还很采，是在一篇博客的指导下完成的。当时未注意到原博客中`--enable-fail-if-missing`这个参数，又照搬人家博客中的配置参数，导致怎么尝试也不好使。最终无奈，特意查询了每个参数的含义后才明白搞清楚是怎么回事。
- 构建失败进如`/user/local/bin`中删除vim，之后再删除之前构建vim的vim目录
