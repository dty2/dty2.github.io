---
title: FTXUI
categories:
- 开发
- linux
tags:
- Arch日记
sticky: 99
category_bar: true
index_img: /images/1.jpg
---
# FTXUI库
{% note success %}
直接阅读源代码是解决所有疑问的最好方式
{% endnote %}
## FTXUI库快速入门
### 简介
dom静态无交互
component动态有交互
dom可以通过`Renderer`提升到component
可以简单理解:
dom = Render()
1. dom + event = component
2. Render() + event = Render() + event
### dom使用流程
1. `screen`
2. `element`
3. `Render(screen, element)`
4. `screen.print()`
### component使用流程
1. `active_screen`
2. `component`
3. `actice_screen.Loop(component)`
### dom变成component后使用流程
1. `active_screen`
2. `auto renderer = Renderer([](){ return element; })`
3. `actice_screen.Loop(renderer)`
## 深入FTXUI库
FTXUI库采用单一消费者多生产者结构，采用了多线程
当调用`active_screen`的`Loop`函数后
会产生两个分支线程，分别负责生产渲染与事件
主线程会进入循环，作为消费者捕获两个分支线程的事件与渲染
进行事件处理时，先父组建会进行捕获，然后交给子活动组件进行处理
待子活动组件处理完后父组建再进行处理
## FAQ
1. 为什么dom变成component之后能显示但是不能操作
[解决方案](https://github.com/ArthurSonzogni/FTXUI/issues/623)
为什么要使用接参的`Renderer(std::function<Element(bool)> render)`?
分析源代码可知，使用这种接收参数的lambda是调用重载函数`Renderer(std::function<Element(bool)> render)`
该函数源代码重写了Focusable这个函数`bool Focusable() const override { return true; }`
而原型函数`Renderer(std::function<Element()> render)`则没有这个实现这个重写
这就会导致由dom提升到component后失去焦点这个问题
由于Container等component的机制导致需要Focusable这个函数的返回值为真才会进行改变
假则不会有任何改变，详细内容见[Container源码](https://arthursonzogni.github.io/FTXUI/container_8cpp_source.html)
