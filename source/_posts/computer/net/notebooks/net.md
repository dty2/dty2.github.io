---
title: net
date: 2024-03-01 18:51:00
tags:
- 学习笔记
---
### 

FDM 频分复用
TDM 时分复用

### 五层协议
应用层 SMTP Telnet HTTP DHCP NFS SNMP DNS POP3 SSH
传输层 TCP UDP HDCP
网络层 IP
链路层 Ethernet(Mac)有线网 Wi-Fi无线网 PPP HDLC Frame Relay
物理层

应用层协议:
|-|-|-|
|应用|协议|下层协议|
|邮件|SMTP|25|TCP|
|邮件|POP3|110||
|远程终端访问|Telnet|TCP|
|远程终端访问|SSH|TCP|
|Web|HTTP|80|TCP|
|文件传输|FTP|20数据连接,21控制连接|TCP|
|远程文件服务器|NFS|通常UDP|
|网络管理|SNMP|通常UDP|
|域名转换|DNS|53|通常UDP|
||||
||||
||||
||||
||||

TCP协议组成:
iCMP 差错报告协议
GBN 退回N步协议 滑动窗口协议 / SR 选择重传

MSS 最大报文段长度
MTU 最大传输单元
