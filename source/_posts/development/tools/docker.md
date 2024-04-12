---
title: Docker&Kubernetes
categories:
- 开发
- 开发工具
tags:
- 虚拟化与容器
category_bar: true
---
* Dockerfile示例: hexo 博客环境
```
FROM alpine:3.14 
ENV http_proxy http://proxy-server:20171
RUN apk add --update --no-cache git nodejs npm
RUN npm install -g hexo-cli hexo-server
RUN addgroup -S blog -g 1000 && \
    adduser -S blog -g '' -u 1000 -G blog && \
    mkdir -p /home/blog/hexo/ && \
    chown blog:blog /home/blog/hexo
USER blog
WORKDIR /home/blog/hexo/
```
* docker-compose示例: hexo博客容器编排
```
version: '3'
services:
  hexo:
    image: blog:alpine3.14
    container_name: blog #容器名称
    restart: always #自动重启,关机后再开机容器自动开启
    user: 1000:1000 #通过gid和uid1000的身份访问volumes
    ports:
      - 4000:4000
    volumes:
      - type: bind
        source: /home/hunter/project/blog/hexo/
        target: /home/blog/hexo/
    stdin_open: true #和tty一起使用,启用容器内sh
    tty: true #和stdin_open一起使用,启用容器内sh
    command: hexo server
    networks:
      - blog-net
networks:
  blog-net:
    driver: bridge
```

# FAQ
* 构建docker镜像会遇到缓存问题，这是上次构件镜像的时候留下的缓存导致
  `docker build --no-cache` 能重新构件，不利用上次的缓存
* 镜像构件时需要本机的代理支持 `docker build --network host`
