---
title: socket
---

socket，阅读手册版
<!-- more -->

强烈建议小伙伴们直接读阅读 [glibc](https://sourceware.org/glibc/manual/latest/pdf/libc.pdf)第16章

手册写的明明白白，并且英文词汇都很简单，像我这种英语菜狗都能流畅阅读

# 16 套接字

套接字，进程间通讯方式的一种，socket像管道一样表述为文件描述符

其不但能在本地通讯，还能在两台不同计算机上通讯

## 16.1 套接字概念

创建一个套接字要指定通讯方式和协议类型

明确通讯方式旨在搞明白(选tcp还是udp)

* 传输单位是啥？是字节序列还是数据包？
* 允不允许丢包？
* 传输对象是一个还是多个？

先选协议簇，再选协议，协议簇也可叫命名空间，也叫"域"

协议簇英文叫protocol family，缩写就是PF，而地址格式英文叫address format，缩写就是AF

在两个程序(两个电脑)之间传输数据需要遵循以下规则

* 两方使用相同协议
* 协议之间的组合都是特定的，如tcp采用字节流通讯方式并且属于Internet命名空间一样
* 对于每个方式的组合和命名空间，都有一个默认的协议，这个默认的协议号就是0，一般使用默认协议就可

开始由于数据类型不统一导致问题，后来POSIX统一规范了，不使用int用size_t，这个size_t在64位机器就是64位

但是用size_t会导致指针变量的问题，后来就都改用socklen_t(最少是32无符号位)

> [!TIP]
> 所谓的指针变量问题就是，本来以前都是32位，后来用size_t虽然兼容了一些内容，但是指针没变
> 即指针指向由32变64，但是指针大小没变(还是原来的32位)

## 16.2 通讯方式

手册说在sys/socket.h中有套接字类型如下

* int SOCK_STREAM 字节流式(tcp)
* int SOCK_DGRAM 数据包(udp)
* int SOCK_RAW 更底层协议(一般程序员用不到)

> [!IMPORTANT]
> 眼见为实，果不其然，翻遍sys/socket.h都没发现这三种套接字类型的声明或定义
> 果然手册都不是什么好东西，最后全局搜索一下才发现原来在bits/socket.h中

``` C
//  NOTE: 以下内容在bits/socket.h文件中

/* Types of sockets.  */
enum __socket_type
{
  SOCK_STREAM = 1,		/* Sequenced, reliable, connection-based
				   byte streams.  */
#define SOCK_STREAM SOCK_STREAM
  SOCK_DGRAM = 2,		/* Connectionless, unreliable datagrams
				   of fixed maximum length.  */
#define SOCK_DGRAM SOCK_DGRAM
  SOCK_RAW = 3,			/* Raw protocol interface.  */
#define SOCK_RAW SOCK_RAW
  SOCK_RDM = 4,			/* Reliably-delivered messages.  */
#define SOCK_RDM SOCK_RDM
  SOCK_SEQPACKET = 5,		/* Sequenced, reliable, connection-based,
				   datagrams of fixed maximum length.  */
#define SOCK_SEQPACKET SOCK_SEQPACKET

#define SOCK_MAX (SOCK_SEQPACKET + 1)
  /* Mask which covers at least up to SOCK_MASK-1.
     The remaining bits are used as flags. */
#define SOCK_TYPE_MASK 0xf

  /* Flags to be ORed into the type parameter of socket and socketpair and
     used for the flags parameter of accept4.  */

  SOCK_CLOEXEC = 0x10000000,	/* Atomically set close-on-exec flag for the
				   new descriptor(s).  */
#define SOCK_CLOEXEC SOCK_CLOEXEC

  SOCK_NONBLOCK = 0x20000000	/* Atomically mark descriptor(s) as
				   non-blocking.  */
#define SOCK_NONBLOCK SOCK_NONBLOCK
};
```

> [!TIP]
> 偶然注意到一个好玩的宏定义
> `# error "Never include <bits/socket.h> directly; use <sys/socket.h> instead."`
> 意思就是说，这个文件不应该直接被包含，我估计是为了隐藏底层的封装防止被像我这样的菜鸟直接包含使用吧

## 16.3 套接字地址

套接字也有名字，其名字就是其地址，因此二者可以混为一谈

刚创建套接字没有地址，就像刚出生的婴儿还没名字一样，因此要给这个套接字搞个名字，由此要用到bind函数

不手动指定套接字地址的话系统会自动分配一个，客户端通常需要指定地址，好来主动连接到服务器

bind和getsockname用于设置和检查套接字的地址，这俩函数用struct sockaddr *来接收

### 16.3.1 地址格式

struct sockaddr 有两个成员函数

* `short int sa_family` 表示协议簇，如AF_INET
* `char sa_data[14]` 表示实际地址数据，如ip地址 + 端口号，实际上并不只14字节，也许更大

源码如下

``` C
//  NOTE: 以下内容在bits/socket.h文件中

/* Structure describing a generic socket address.  */
struct __attribute_struct_may_alias__ sockaddr
  {
    __SOCKADDR_COMMON (sa_);	/* Common data: address family and length.  */
    char sa_data[14];		/* Address data.  */
  };

//  NOTE: 以下内容在bits/cdefs.h文件中

/* Mark struct types as aliasable.  Restricted to compilers that
   support forward declarations of structs in the presence of the
   attribute.  */
#if __GNUC_PREREQ (7, 1) || defined __clang__
# define __attribute_struct_may_alias__ __attribute__ ((__may_alias__))
#else
# define __attribute_struct_may_alias__
#endif

//  NOTE: 以下内容在bits/sockaddr.h文件中

/* POSIX.1g specifies this type name for the `sa_family' member.  */
typedef unsigned short int sa_family_t;

/* This macro is used to declare the initial common members
   of the data types used for socket addresses, `struct sockaddr',
   `struct sockaddr_in', `struct sockaddr_un', etc.  */

#define	__SOCKADDR_COMMON(sa_prefix) \
  sa_family_t sa_prefix##family
```

> [!TIP]
> __attribute__，编译器关键字，用于向编译器提供额外信息或指示，多用于优化
> 这里是将sockaddr标记为可别名，即通过不同的类型访问同一块内存
> 例如，网络协议栈中可能需要将同一块内存解释为不同的结构体类型。可别名属性可以帮助解决这些问题

对于第一个成员变量，传入选择的协议簇，有以下选项

* AF_LOCAL: 用于本地命名空间(本地进程间通讯)
* AF_UNIX: AF_LOCAL的代名词，虽然AF_LOCAL也是POSIX要求的，但是AF_UNIX兼容性更强
* AF_FILE: AF_LOCAL的同义词，和AF_UNIX一样，都是为了兼容
* AF_INET: Internet命名空间，IPv4用
* AF_INET6: Internet命名空间，IPv6用
* AF_UNSPEC: 不咋用，清除已经连接的套接字用的

当然，依旧需要看看源码实现心里才踏实

``` C
//  NOTE: 以下内容在bits/socket.h文件中

/* Protocol families.  */
#define PF_UNSPEC	0	/* Unspecified.  */
#define PF_LOCAL	1	/* Local to host (pipes and file-domain).  */
#define PF_UNIX		PF_LOCAL /* POSIX name for PF_LOCAL.  */
#define PF_FILE		PF_LOCAL /* Another non-standard name for PF_LOCAL.  */
#define PF_INET		2	/* IP protocol family.  */
#define PF_AX25		3	/* Amateur Radio AX.25.  */
#define PF_IPX		4	/* Novell Internet Protocol.  */
#define PF_APPLETALK	5	/* Appletalk DDP.  */
#define PF_NETROM	6	/* Amateur radio NetROM.  */
#define PF_BRIDGE	7	/* Multiprotocol bridge.  */
#define PF_ATMPVC	8	/* ATM PVCs.  */
#define PF_X25		9	/* Reserved for X.25 project.  */
#define PF_INET6	10	/* IP version 6.  */
#define PF_ROSE		11	/* Amateur Radio X.25 PLP.  */
#define PF_DECnet	12	/* Reserved for DECnet project.  */
#define PF_NETBEUI	13	/* Reserved for 802.2LLC project.  */
#define PF_SECURITY	14	/* Security callback pseudo AF.  */
#define PF_KEY		15	/* PF_KEY key management API.  */
#define PF_NETLINK	16
#define PF_ROUTE	PF_NETLINK /* Alias to emulate 4.4BSD.  */
#define PF_PACKET	17	/* Packet family.  */
#define PF_ASH		18	/* Ash.  */
#define PF_ECONET	19	/* Acorn Econet.  */
#define PF_ATMSVC	20	/* ATM SVCs.  */
#define PF_RDS		21	/* RDS sockets.  */
#define PF_SNA		22	/* Linux SNA Project */
#define PF_IRDA		23	/* IRDA sockets.  */
#define PF_PPPOX	24	/* PPPoX sockets.  */
#define PF_WANPIPE	25	/* Wanpipe API sockets.  */
#define PF_LLC		26	/* Linux LLC.  */
#define PF_IB		27	/* Native InfiniBand address.  */
#define PF_MPLS		28	/* MPLS.  */
#define PF_CAN		29	/* Controller Area Network.  */
#define PF_TIPC		30	/* TIPC sockets.  */
#define PF_BLUETOOTH	31	/* Bluetooth sockets.  */
#define PF_IUCV		32	/* IUCV sockets.  */
#define PF_RXRPC	33	/* RxRPC sockets.  */
#define PF_ISDN		34	/* mISDN sockets.  */
#define PF_PHONET	35	/* Phonet sockets.  */
#define PF_IEEE802154	36	/* IEEE 802.15.4 sockets.  */
#define PF_CAIF		37	/* CAIF sockets.  */
#define PF_ALG		38	/* Algorithm sockets.  */
#define PF_NFC		39	/* NFC sockets.  */
#define PF_VSOCK	40	/* vSockets.  */
#define PF_KCM		41	/* Kernel Connection Multiplexor.  */
#define PF_QIPCRTR	42	/* Qualcomm IPC Router.  */
#define PF_SMC		43	/* SMC sockets.  */
#define PF_XDP		44	/* XDP sockets.  */
#define PF_MCTP		45	/* Management component transport protocol.  */
#define PF_MAX		46	/* For now..  */

/* Address families.  */
#define AF_UNSPEC	PF_UNSPEC
#define AF_LOCAL	PF_LOCAL
#define AF_UNIX		PF_UNIX
#define AF_FILE		PF_FILE
#define AF_INET		PF_INET
#define AF_AX25		PF_AX25
#define AF_IPX		PF_IPX
#define AF_APPLETALK	PF_APPLETALK
#define AF_NETROM	PF_NETROM
#define AF_BRIDGE	PF_BRIDGE
#define AF_ATMPVC	PF_ATMPVC
#define AF_X25		PF_X25
#define AF_INET6	PF_INET6
#define AF_ROSE		PF_ROSE
#define AF_DECnet	PF_DECnet
#define AF_NETBEUI	PF_NETBEUI
#define AF_SECURITY	PF_SECURITY
#define AF_KEY		PF_KEY
#define AF_NETLINK	PF_NETLINK
#define AF_ROUTE	PF_ROUTE
#define AF_PACKET	PF_PACKET
#define AF_ASH		PF_ASH
#define AF_ECONET	PF_ECONET
#define AF_ATMSVC	PF_ATMSVC
#define AF_RDS		PF_RDS
#define AF_SNA		PF_SNA
#define AF_IRDA		PF_IRDA
#define AF_PPPOX	PF_PPPOX
#define AF_WANPIPE	PF_WANPIPE
#define AF_LLC		PF_LLC
#define AF_IB		PF_IB
#define AF_MPLS		PF_MPLS
#define AF_CAN		PF_CAN
#define AF_TIPC		PF_TIPC
#define AF_BLUETOOTH	PF_BLUETOOTH
#define AF_IUCV		PF_IUCV
#define AF_RXRPC	PF_RXRPC
#define AF_ISDN		PF_ISDN
#define AF_PHONET	PF_PHONET
#define AF_IEEE802154	PF_IEEE802154
#define AF_CAIF		PF_CAIF
#define AF_ALG		PF_ALG
#define AF_NFC		PF_NFC
#define AF_VSOCK	PF_VSOCK
#define AF_KCM		PF_KCM
#define AF_QIPCRTR	PF_QIPCRTR
#define AF_SMC		PF_SMC
#define AF_XDP		PF_XDP
#define AF_MCTP		PF_MCTP
#define AF_MAX		PF_MAX
```

好吧，虽然这么多，但是手册上说

``` markdown
sys/socket.h defines symbols starting with ‘AF_’ for many different kinds of networks,
most or all of which are not actually implemented. We will document those that really work
as we receive information about how to use them.
```

### 16.3.2 设置套接字的地址

用来给套接字设置地址的函数就是bind，函数原型就在sys/socket.h，这回手册确实没骗人，函数原型确实在sys/socket.h

``` C
//  NOTE: 以下内容在socket/sys/socket.h文件中

/* Give the socket FD the local address ADDR (which is LEN bytes long).  */
extern int bind (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len)
     __THROW;
```
bind是3S的，即`MT-Safe | AS-Safe | AC-Safe` 多线程安全，异步信号安全，异步取消安全

bind的返回值正常是0，失败是-1，并且全局变量errno会被设为以下值

* EBADF: 套接字参数是无效文件描述符
* ENOTSOCK: 不是套接字
* EADDRNOTAVAIL: 地址不可用
* EADDRINUSE: 地址被占用
* EINVAL: 套接字已有地址
* EACCES: 没权限访问地址

### 16.3.1 读取套接字的地址

可以用getsockname去检查internet的地址

getsockname也是3S的

``` C
//  NOTE: 以下内容在socket/sys/socket.h文件中

/* Put the local address of FD into *ADDR and its length in *LEN.  */
extern int getsockname (int __fd, __SOCKADDR_ARG __addr,
			socklen_t *__restrict __len) __THROW;
```

getsockname的返回值正常是0，失败是-1，并且全局变量errno会被设为以下值

* EBADF: 套接字参数是无效文件描述符
* ENOTSOCK: 不是套接字
* ENOBUFS: 缓冲区不足

## 16.4 接口名字

网络接口有名字，如lo(loopback)，eth0(first ethernet interface)第一个以太网接口

直接用名字比较费劲，因此用随机分配的索引来代替，索引是个小正整数

接口的名字与索引由以下数据结构与类型表示

* `size_t IFNAMSIZ` 用来表示名字字符数量(大小)
* `unsigned int if_nametoindex(const chat *ifname)` 名字转成索引
* `char *if_indextoname(unsigned int ifindex, char *ifname)` 索引转成名字
* `struct if_nameindex` 包含接口和名字两个成员变量
* `struct if_nameindex *if_nameindex(void)` 获得struct if_nameindex结构(即new struct if_nameindex)
* `void if_freenameindex(struct if_nameindex * ptr)` 释放struct if_nameindex结构(即delete struct id_nameindex)

## 16.5 本地命名空间

本地命名空间，也就是PF_LOACAL协议簇，本节就是围绕这个展开

本地命名空间众所周知为"Unix domain sockets"，说中文就是Unix套接字域，别名就是文件命名空间

之所以叫文件命名空间是因为在本地命名空间中，套接字地址一般使用文件名

### 16.5.1 本地命名空间概念

本地命名空间的套接字地址就是文件名，可以指定任何有写权限的文件，其一般都在/tmp目录

本地命名空间的特点在于，只有在打开连接的那一刻有用，当打开连接之后就没意义了，并且可能都不存在

并且，本地命名空间的最大特点就在于不能跨机器使用，即使是共享文件系统(挂载)也不行

虽然当前有的程序利用了这一点来区分不同的机器(客户端)，但是并不建议你再设计相关程序利用这一特性了，因为这一特性在未来可能消失

### 16.5.2 本地命名空间细节

创建本地命名空间使用常量PF_LOACAL作为传递给socket的参数，其在[地址格式](#1631-地址格式)的源码部分展示了

具体结构体名在sys/un.h，源码如下，部分宏依旧参考[地址格式](#1631-地址格式)的源码部分

``` C
//  NOTE: 以下内容在socket/sys/un.h文件中

/* Structure describing the address of an AF_LOCAL (aka AF_UNIX) socket.  */
struct __attribute_struct_may_alias__ sockaddr_un
  {
    __SOCKADDR_COMMON (sun_);
    char sun_path[108];		/* Path name.  */
  };
```
* 第一个参数是协议簇地址格式，例如AF_LOCAL
* 第二个参数是文件名

`int SUN_LEN(struct sockaddr_un * ptr)`宏可以计算struct sockaddr_un的具体大小

``` C
//  NOTE: 以下内容在socket/sys/un.h文件中

/* Evaluate to actual length of the `sockaddr_un' structure.  */
# define SUN_LEN(ptr) (offsetof (struct sockaddr_un, sun_path)		      \
		      + strlen ((ptr)->sun_path))
#endif
```

### 16.5.3 本地命名空间套接字用例

``` C
#include <stddef.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

int make_named_socket (const char *filename) {
  struct sockaddr_un name;
  int sock;
  size_t size;

  /* Create the socket. */
  sock = socket(PF_LOCAL, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* Bind a name to the socket. */
  name.sun_family = AF_LOCAL;
  strncpy(name.sun_path, filename, sizeof(name.sun_path));
  name.sun_path[sizeof(name.sun_path) - 1] = '\0';

  /* The size of the address is the offset of the start of the filename,
   * plus its length (not including the terminating null byte).
   * Alternatively you can just do: size = SUN_LEN(&name);
   */
  size = (offsetof(struct sockaddr_un, sun_path) + strlen(name.sun_path));
  if(bind(sock, (struct sockaddr *) &name, size) < 0) {
    perror("bind");
    exit( EXIT_FAILURE);
  }
  return sock;
}
```

## 16.6 互联网命名空间

起初互联网命名空间都用IPv4，后来IP地址不够用，又搞了个IPv6，v4是32位，v6是128位

IPv4用PF_INET，IPv6用PF_INET6，其源码在[地址格式](#1631-地址格式)的源码部分展示了

其套接字地址包含IP地址和端口号，并且需要保证ip地址和端口号按网络字节序表示

### 16.6.1 互联网套接字地址格式

地址类型就两种分别是AF_INET和AF_INET6，并且由主机地址和主机端口号组成

此外，选择的协议也就作为了地址的一部分，因为只有在特定的协议，本地端口号才其作用

关于互联网命名空间的数据类型都在netinet/in.h

``` C
//  NOTE: 以下内容在inet/netinet/in.h文件中

/* Structure describing an Internet socket address.  */
struct __attribute_struct_may_alias__ sockaddr_in
  {
    __SOCKADDR_COMMON (sin_); // NOTE: ==> sa_family_t sin_family; ==> unsigned short int sin_family;

    in_port_t sin_port;			/* Port number.  */
    struct in_addr sin_addr;		/* Internet address.  */

    /* Pad to size of `struct sockaddr'.  */
    unsigned char sin_zero[sizeof (struct sockaddr)
			   - __SOCKADDR_COMMON_SIZE
			   - sizeof (in_port_t)
			   - sizeof (struct in_addr)];
  };

#if __USE_KERNEL_IPV6_DEFS
struct __attribute_struct_may_alias__ sockaddr_in6;
#else
/* Ditto, for IPv6.  */
struct __attribute_struct_may_alias__ sockaddr_in6
  {
    __SOCKADDR_COMMON (sin6_);
    in_port_t sin6_port;	/* Transport layer port # */
    uint32_t sin6_flowinfo;	/* IPv6 flow information */
    struct in6_addr sin6_addr;	/* IPv6 address */
    uint32_t sin6_scope_id;	/* IPv6 scope-id */
  };
#endif /* !__USE_KERNEL_IPV6_DEFS */
```

IPv4没啥好说的，就是当调用bind和getsockname的时候需要使用sizeof(struct sockaddr_in)

对于IPv6，有以下参数

* 协议簇，没啥好说的，传入AF_INET6
* 端口，也没啥好说的，传输层的端口
* 地址，依旧没啥好说的，IPv6的地址
* 流信息，包含流量类和流标签值，存在IPv6的头部中，本字段按网络字节序存储，只有低28位被使用，其余必须全为0，其中低20位是流标签，20到27是流量类，通常这个字段为0

### 16.6.2 主机地址

很显然，根据IPv4和IPv6来看，有两种地址，并且主机也有名字，比如叫"www.gun.org"

#### 16.6.2.1 互联网主机地址

> [!TIP]
> 温馨提示，本小节讲解了IPv4和IPv6地址相关内容，学过计网的童鞋们可以自行跳过

IPv4，32位，4字节。历史上讲，其地址分为两部分，分别为网络地址和本地地址

在20世纪90年代中期改变了这一情况，引入了无类别地址，但是由于一些函数隐式要求这些旧的规定，因此对于有类别和无类别需要分开讨论

IPv6就只使用无类别地址，因此接下来的内容对IPv6是无用的

IPv4的地址的前1,2或3字节为网络地址，其他的为本地地址

网络地址由Network Information Center(NIC)分配，其并且分为ABC三类，而本地地址则由特定网络的网管分配

* A类网络地址有1字节，从0到127。特点就是少，但是每个网络可以由很多主机
* B类网络地址有2字节，第一个字节从128到191
* C类网络最小，其有3字节，第一字节从192到255

A类网络的0.0.0.0为广播地址，目前已经过时，为了兼任不应该使用，并且127.0.0.1为回环地址

一个机器能有多个网络地址，但不能多个机器有一个网络地址

IPv4一般用点分法表示，分别是a.b.c.d / a.b.c / a.b / a

> [!NOTE]
> 点分法英文叫"numbers-and-dots"，本来这挺好理解的，结果翻译成中文就成"点分法"，不知道还以为多高级

但是！现在不用有类别地址，都用无类别地址，因此引入了子网掩码

IPv6地址是128位，通常用被冒号(colon)分割的16位16进制(hexadecimal)表示

双冒号即::用来表示连续的0，例如0:0:0:0:0:0:0:1也可表示为::1

#### 16.2.2.2 主机地址数据类型

主机地址有时候表示为uint32_t，其他情况表示为struct in_addr，虽然统一固定的形式更好，但是其实从结构体提取出来整数也并不难

好吧，这里手册上写了巴拉巴拉一大堆，实际上就是关于32到64位变革中使用unsigned long int这样的表示方式带来的一些问题，对像我这样的菜鸟来说，没啥好看的这里

对于IPv6，就表示为struct in6_addr

``` C
//  NOTE: 以下内容在inet/netinet/in.h文件中

/* Internet address.  */
typedef uint32_t in_addr_t;
struct in_addr
  {
    in_addr_t s_addr;
  };

// ...

/* Address to accept any incoming messages.  */
#define	INADDR_ANY		((in_addr_t) 0x00000000)
/* Address to send to all hosts.  */
#define	INADDR_BROADCAST	((in_addr_t) 0xffffffff)
/* Address indicating an error return.  */
#define	INADDR_NONE		((in_addr_t) 0xffffffff)
/* Dummy address for source of ICMPv6 errors converted to IPv4 (RFC
   7600).  */
#define	INADDR_DUMMY		((in_addr_t) 0xc0000008)

/* Network number for local host loopback.  */
#define	IN_LOOPBACKNET		127
/* Address to loopback in software to local host.  */
#ifndef INADDR_LOOPBACK
# define INADDR_LOOPBACK	((in_addr_t) 0x7f000001) /* Inet 127.0.0.1.  */
#endif

// ...

#if !__USE_KERNEL_IPV6_DEFS
/* IPv6 address */
struct in6_addr
  {
    union
      {
	uint8_t	__u6_addr8[16];
	uint16_t __u6_addr16[8];
	uint32_t __u6_addr32[4];
      } __in6_u;
#define s6_addr			__in6_u.__u6_addr8
#ifdef __USE_MISC
# define s6_addr16		__in6_u.__u6_addr16
# define s6_addr32		__in6_u.__u6_addr32
#endif
  };
#endif /* !__USE_KERNEL_IPV6_DEFS */

extern const struct in6_addr in6addr_any;        /* :: */
extern const struct in6_addr in6addr_loopback;   /* ::1 */

//  NOTE: 以下内容在inet/in6_addr.c文件中
const struct in6_addr __in6addr_any =
{ { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } } };
libc_hidden_data_def (__in6addr_any)
weak_alias (__in6addr_any, in6addr_any)
libc_hidden_data_weak (in6addr_any)
const struct in6_addr __in6addr_loopback =
{ { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } } };
libc_hidden_data_def (__in6addr_loopback)
weak_alias (__in6addr_loopback, in6addr_loopback)
libc_hidden_data_weak (in6addr_loopback)
```

#### 16.6.2.3 主机地址函数

以下函数声明在arpa/inet.h用来操纵(manipulate)互联网地址

* `int inet_aton(const char *name, struct in_addr *addr)`
  a(address) to n(notation)，将地址转成符号存放到struct in_addr中(如127.0.0.1)，返回值为0表示成功，非0为地址无效
* `uint32_t inet_addr(const char *name)`
  只将地址转成符号，地址无效则返回INADDR_NONE，这个函数相对于inet_aton来说是过时的(obsolete)，其之所以过时就是因为INADDR_NONE(255.255.255.255)这个地址是有效的
* `uint32_t inet_network(const char *name)` 从整个地址获取网络地址部分
* `char *inet_ntoa(struct in_addr addr)`
  n(notation) to a(address) 将符号转成地址，返回一个静态分配的缓冲区，再次调用会复写当前的缓冲区，如果需要保存字符串则需要单独拷贝出来，在多线程中每个线程都有一个独立的静态缓冲区，不过在统一线程中再次调用也会发生复写，应该使用inet_ntop，因为inet_ntop能处理IPv4或IPv6
* `struct in_addr inet_makeaddr(uint32_t net, uint32_t local)` 将网络地址和主机地址合并
* `uint32_t inet_lnaof(struct in_addr addr)` 返回主机地址，但是因为使用在有类地址中，因此不应再使用
* `uint32_t inet_netof(struct in_addr addr)` 返回网络地址，但是因为使用在有类地址中，因此不应再使用
* `int inet_pton(int af, const char *cp, void *buf)`
  将地址转成符号，af是AF_INET或者是AF_INET6表示期望转换成的IP版本，cp表示地址，buf表示符号
* `const char *inet_ntop(int af, const void *cp, char *buf, socklen_t)`
  将符号转成地址，af是AF_INET或者是AF_INET6表示期望转换成的IP版本，cp表示地址，buf表示符号

以上函数都是3S

#### 16.6.2.4 主机名称

主机名称就是俗话说的网址，好处就是相对于IP地址来说更方便记

系统用数据库保存主机名称和主机地址之间的映射，一般在/etc/hosts或等价的提供名称服务的服务器中

相关的函数和数据类型在netdb.h中声明

``` C
//  NOTE: 以下内容在resolv/netdb.h文件中

/* Description of data base entry for a single host.  */
struct hostent
{
  char *h_name;			/* Official name of host.  */
  char **h_aliases;		/* Alias list.  */
  int h_addrtype;		/* Host address type.  */
  int h_length;			/* Length of address.  */
  char **h_addr_list;		/* List of addresses from name server.  */
#ifdef __USE_MISC
# define	h_addr	h_addr_list[0] /* Address, for backward compatibility.*/
#endif
};
```

> [!NOTE]
> 上面几个成员函数没啥好说的，注释都在那摆着呢，有啥不懂的去看手册吧

可以用gethostbyname，gethostbyname2或者是gethostbyaddr去搜索指定的主机名称，返回值存在静态分配的结构体中，再次调用会复写，因此需额外保存，同时可以用getaddrinfo和getnameinfo去获取这些信息

* `struct hostent *gethostbyname(const char *name)` 通过主机名找地址，成功返回主机名称，解析失败则是空指针
* `struct hostend *gethostbyname2(const char *name, int af)` 和gethostbyname功能一样，就能能指定协议簇
* `struct hostend *gethostbyaddr(const void *addr, socklen_t length, int format)` 顾名思义，通过地址获得主机地址

以上函数查询失败，则可查看h_errno寻找原因，可能的值如下

* HOST_NOT_FOUND 在数据库中找不到主机
* TRY_AGAIN 连不上名称服务器，再试试应该就没问题了
* NO_RECOVERY 发生了不可恢复的错误
* NO_ADDRESS 有这个名称的条目，但是没有关联到IP地址

以上函数都不可重入，没法在多线程中使用，因此有了以下新函数

> [!TIP]
> 温馨提示，可重入意味着任何时候都可以被中断，并且在中断后可被安全地调用

* `int gethostbyname_r(const char *restrict name, struct hostent *restrict result_buf, char *restrict buf, size_t buflen, struct hostent **restrict result, int *restrict h_errnop)`
* `int gethostbyname2_r (const char *name, int af, struct hostent *restrict result_buf, char *restrict buf, size_t buflen, struct hostent **restrict result, int *restrict h_errnop)`
* `int gethostbyaddr_r (const void *addr, socklen_t length, int format, struct hostent *restrict result_buf, char *restrict buf, size_t buflen, struct hostent **restrict result, int *restrict h_errnop)`

以上三个函数，一个比一个复杂，懒得看手册这部分的内容了，有需要再看手册吧

以下三个函数可扫描整个主机地址数据库，但他们都是不可重入的

* `void sethostent(int stayopen)` 打开主机数据库并开始扫描，接下来使用函数gethostent来获取条目
* `struct hostent * gethostent(void)` 返回下一个条目，如果没有则返回空指针
* `void endhost(void)` 关闭主机数据库

### 16.6.3 互联网端口

端口，值为0到65535，小于IPPORT_RESERVED的端口是保留给标准服务器的，例如finger和telnet，使用getservbyname函数可以映射服务名到指定端口

如果使用一个没有指定地址的socket，系统会为其生成一个端口号，值在IPPORT_RESERVED和IPPORT_USERRESERVED范围之间

在互联网上事实上允许两个套接字有相同的端口，只要他们永远不去和相同套接字地址(ip+port)的套接字去通讯就行

上面的话说成大白话就是，一个计算机，允许有俩进程，共用一个套接字，只要这俩之间不进行通讯就行

正常来说不应该这么做，除非应用层的协议有特殊要求，一般系统不会让你这么做，bind通常坚持使用不同的端口号

要想重用端口，需要设置套接字选项SO_REUSEADDR

### 16.6.4 服务数据库

跟踪知名服务的数据库通常是/etc/services(应用层协议，换句话说就叫服务)文件或者是等价的名称服务器

有以下数据结构用来进行操作

``` C
//  NOTE: 以下内容在bits/netdb.h文件中

/* Description of data base entry for a single service.  */
struct servent
{
  char *s_name;			/* Official service name.  */
  char **s_aliases;		/* Alias list.  */
  int s_port;			/* Port number.  */
  char *s_proto;		/* Protocol to use.  */
};
```

获得特定服务的信息可以使用getservbyname或getservbyport函数，这些函数返回静态分配的结构，会复写需要转存

* `struct servent * getservbyname(const char *name, const char *proto)` 根据服务名获取服务信息
* `struct servent * getservbyport(int port, const char *proto)` 根据服务端口获取服务信息

也可以使用setservent,getservent和endservent来扫描服务数据库，这些服务都是不可重入的

* `void setservent(int stayopen)` 打开服务数据库并且扫描，如果传参为非零，则会保持打开，通常情况下就关了
* `struct servent * getservent(void)` 返回服务数据库的下一个条目，没有则返回空指针
* `void endservent(void)` 关闭服务数据库

### 16.6.5 字节序转换

有的电脑用大端有的用小端，为了网络传输，搞了各网络字节序来统一

如果使用getservbyname和gethostbyname或者是inet_addr去获取端口和主机地址，获取的值就是网络字节序的，可以直接拷贝他们到sockaddr_in中

要不然就得手动转换，使用htons和ntons函数转换sin_port成员的值

使用htonl和ntohl去转换IPv4地址给sin_addr成员(记住struct in_addr等价于uint32_t)

* `uint16_t htons(uint16_t hostshort)` 将hostshort从主机字节序转为网络字节序，16位无符号
* `uint16_t ntohs(uint16_t netshort)` 将hostshort从网络字节序转为主机字节序，16位无符号
* `uint32_t htonl(uint32_t hostlong)` 将hostshort从主机字节序转为网络字节序，32位无符号，用在IPv4中
* `uint32_t ntohl(uint32_t netlong)` 将hostshort从网络字节序转为主机字节序，32位无符号，用在IPv4中

``` C
//  NOTE: 以下内容在inet/netinet/netdb.h文件中

/* Functions to convert between host and network byte order.

   Please note that these functions normally take `unsigned long int' or
   `unsigned short int' values as arguments and also return them.  But
   this was a short-sighted decision since on different systems the types
   may have different representations but the values are always the same.  */

extern uint32_t ntohl (uint32_t __netlong) __THROW __attribute__ ((__const__));
extern uint16_t ntohs (uint16_t __netshort)
     __THROW __attribute__ ((__const__));
extern uint32_t htonl (uint32_t __hostlong)
     __THROW __attribute__ ((__const__));
extern uint16_t htons (uint16_t __hostshort)
     __THROW __attribute__ ((__const__));
```

### 16.6.6 协议数据库

在/etc/protocols中能看到这台电脑支持什么协议，同时名称服务器也可提供，其有以下相关数据结构

``` C
//  NOTE: 以下内容在resolv/netdb.h文件中

/* Description of data base entry for a single service.  */
struct protoent
{
  char *p_name;			/* Official protocol name.  */
  char **p_aliases;		/* Alias list.  */
  int p_proto;			/* Protocol number.  */
};
```

可以通过以下函数在整个协议库中搜索指定协议 ，这些函数返回静态分配的结构，会复写需要转存

* `struct protoent * getprotobyname(const char *name)`  通过协议名返回协议信息，没有则返回空指针
* `struct protoent * getprotobynumber(int protocol)` 通过协议号返回协议信息，没有则返回空指针

可以通过setprotoent, getprotoent和endprotoent扫描整个协议数据库，注意这些函数是不可重入的

* `void setprotoent(int stayopen)` 打开协议数据库并且扫描，如果传参为非零，则会保持打开，通常情况下就关了
* `struct protoent * getprotoent(void)` 返回协议数据库的下一个条目，没有则返回空指针
* `void endprotoent(void)` 关闭协议数据库

### 16.6.7 互联网套接字例程

``` C
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

int make_socket(uint16_t port) {
  int sock;
  struct sockaddr_in name;

  /* Create the socket. */
  sock = socket(PF_INET, SOCK_STREAM, 0);
  if(sock < 0 ) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* Give the socket a name. */
  name.sin_family = AF_INET;
  name.sin_port = htons(port);
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(sock, (struct sockaddr *) &name, sizeof(name)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  return sock;
}
```

另一个版本

``` C
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void init_sockaddr(struct sockaddr_in *name, const char *hostname, uint16_t port) {
  struct hostent *hostinfo;

  name->sin_family = AF_INET;
  name->sin_port = htons(port);
  hostinfo = gethostbyname(hostname);
  if(hostinfo == NULL) {
    fprintf(stderr, "Unknown host %s. \n", hostname);
    exit(EXIT_FAILURE);
  }
  name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
}
```

## 16.7 其他命名空间

不常用，没文档

## 16.8 开关套接字

开关套接字，以及相同的函数适用于所有命名空间和连接方式

### 16.8.1 创建套接字

``` C
//  NOTE: 以下内容在socket/sys/socket.h文件中

/* Create a new socket of type TYPE in domain DOMAIN, using
   protocol PROTOCOL.  If PROTOCOL is zero, one is chosen automatically.
   Returns a file descriptor for the new socket, or -1 for errors.  */
extern int socket (int __domain, int __type, int __protocol) __THROW;
```

三个参数分别是命名空间，类型，协议；命名空间就是PF_LOCAL或PF_INET，类型就是SOCK_STREAM啥的，协议基本都是默认，也就是0

返回值为-1则不正常，errno会被设为如下值

* ENDPROTOENT: 协议或者类型不被命名空间支持
* EMFILE: 这个进程文件描述符已经开到上限
* ENFILE: 这个系统文件描述符已经开到上限
* EACCES: 没权限访问地址
* ENOBUFS: 缓冲区不足

返回的这个文件描述符说白了就是套接字，支持读写，但就像管道一样，不支持文件定位操作

> [!TIP]
> 文件定位操作，就是在文件读写的时候移动读写指针的位置

### 
