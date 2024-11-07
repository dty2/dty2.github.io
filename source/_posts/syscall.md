---
title: 系统调用
sticky: 500
---

记录学习系统调用过程
<!-- more -->

# 系统调用

好奇想知道系统调用到底是啥样，因此有了下文

> [!TIP]
> 底层探究之首，将重点放在"系统调用"上

## 前期准备

### 源码

- [glibc源码获取](https://sourceware.org/glibc/started.html)(版本：2.40)
- [linux源码仓库](https://github.com/torvalds/linux)(版本：6，6点几忘了)
- 示例代码如下

``` C
// NOTE: fork.c

#include <unistd.h>

int main () {
  fork();
  return 0;
}
```

### 工具

需要一堆工具，缺啥装啥吧，实验环境是在arch linux，因此装啥都方便

## 库源码阅读

### 封装函数

`gcc -g fork.c`生成可执行文件用于gdb调试，之后`gdb -tui a.out`看看backtrace

``` gdb
(gdb) layout split # 看源码同时看汇编
(gdb) catch syscall # catch能捕捉事件发生，比如系统调用，动态库加/卸载，抛异常
(gdb) backtrace # 看看函数调用栈
```

发现函数调用栈是`arch_fork() <- _Fork() <- fork() <- main()`

得知`arch_fork`函数是陷入内核前最后执行的一个函数

在glibc中找到arch_fork后，通过观察源码不难发现`INLINE_SYSCALL_CALL (clone, flags, 0, NULL, ctid, 0);`

解析这个宏，发现解析成这个

``` C
// NOTE: unix/sysdep.h

#define __INLINE_SYSCALL6(name, a1, a2, a3, a4, a5, a6) \
  INLINE_SYSCALL (name, 6, a1, a2, a3, a4, a5, a6)
```

> [!TIP]
> 这里其实跳过了一大步，通过各种尝试，发现猜测和`gcc -E`两种方法推断宏比较靠谱

### 解析宏

如果此时尝试再次跳转到定义，就会发现跳转到文件末尾的这里

``` C
// NOTE: unix/sysdep.h

/* Wrappers around system calls should normally inline the system call code.
   But sometimes it is not possible or implemented and we use this code.  */
#ifndef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...) __syscall_##name (args)
#endif
```

本人就掉到这个坑里，没注意到ifndef，最后靠偶遇一篇博客解决的

真正的宏在头文件`#include <sysdep.h>`中

``` C
// NOTE: linux/sysdep.h

/* Define a macro which expands into the inline wrapper code for a system
   call.  It sets the errno and returns -1 on a failure, or the syscall
   return value otherwise.  */
#undef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...)				\
  ({									\
    long int sc_ret = INTERNAL_SYSCALL (name, nr, args);		\
    __glibc_unlikely (INTERNAL_SYSCALL_ERROR_P (sc_ret))		\
    ? SYSCALL_ERROR_LABEL (INTERNAL_SYSCALL_ERRNO (sc_ret))		\
    : sc_ret;								\
  })
```

> [!TIP]
> 这里依旧跳过了一个重要的问题：怎么知道这个宏定没定义
> 其实也很简单，在示例代码中加一段
> 
> ``` C
> #include <stdio.h>
> #include <unistd.h>
> 
> #ifndef INLINE_SYSCALL
> #define ok 1
> #else
> #define ok 2
> #endif
> 
> int main () {
>   fork();
>   printf("%d\n", ok);
>   return 0;
> }
> ```

接下来找到这里

``` C
// NOTE: x86_64/sysdep.h

#undef internal_syscall6
#define internal_syscall6(number, arg1, arg2, arg3, arg4, arg5, arg6) \
({									\
    unsigned long int resultvar;					\
    TYPEFY (arg6, __arg6) = ARGIFY (arg6);			 	\
    TYPEFY (arg5, __arg5) = ARGIFY (arg5);			 	\
    TYPEFY (arg4, __arg4) = ARGIFY (arg4);			 	\
    TYPEFY (arg3, __arg3) = ARGIFY (arg3);			 	\
    TYPEFY (arg2, __arg2) = ARGIFY (arg2);			 	\
    TYPEFY (arg1, __arg1) = ARGIFY (arg1);			 	\
    register TYPEFY (arg6, _a6) asm ("r9") = __arg6;			\
    register TYPEFY (arg5, _a5) asm ("r8") = __arg5;			\
    register TYPEFY (arg4, _a4) asm ("r10") = __arg4;			\
    register TYPEFY (arg3, _a3) asm ("rdx") = __arg3;			\
    register TYPEFY (arg2, _a2) asm ("rsi") = __arg2;			\
    register TYPEFY (arg1, _a1) asm ("rdi") = __arg1;			\
    asm volatile (							\
    "syscall\n\t"							\
    : "=a" (resultvar)							\
    : "0" (number), "r" (_a1), "r" (_a2), "r" (_a3), "r" (_a4),		\
      "r" (_a5), "r" (_a6)						\
    : "memory", REGISTERS_CLOBBERED_BY_SYSCALL);			\
    (long int) resultvar;						\
})
```

内联汇编，在解析宏的途中不难得知，number就是系统调用号

即`__NR_clone`，也就是`#define __NR_clone 56`，在arch-syscall.h中可以找到

通过查询[文档](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html?wapkw=x86-64%20instruction%20set)得知syscall指令的行为

> RCX := RIP; 保存rip
> RIP := IA32_LSTAR; 将rip设置为系统调用入口地址
> R11 := RFLAGS; 保存标志寄存器
> RFLAGS := RFLAGS AND NOT(IA32_FMASK); 清空标志寄存器的一些位
> CPL := 0; 设置保护模式为0，即内核模式

其中IA32_LSTAR是跳转关键，即系统调用内核的入口

IA32_LSTAR是MSR寄存器，即模型特定寄存器，专门用来存放系统调用入口地址，在内核启动时会对其进行初始化

简单总结一句就是，系统调用被glibc封装，在用户空间本质上做的是宏封装内联汇编传参

## 内核剖析

### 寻找入口

进入内核前有必要看看内核文档

查看文档，直接搜syscall很容易找到[这里](https://www.kernel.org/doc/html/latest/core-api/entry.html#syscalls)

文档上写的很清楚，系统调用入口代码从汇编代码开始，在建立特定于架构的低级状态和堆栈框架后调用低级C代码

但是这里依旧不知道系统调用入口在哪里，不过现在已经知道系统调用的样子大致如下

``` C
// NOTE: 取自内核文档
noinstr void syscall(struct pt_regs *regs, int nr)
{
      arch_syscall_enter(regs);
      nr = syscall_enter_from_user_mode(regs, nr);

      instrumentation_begin();
      if (!invoke_syscall(regs, nr) && nr != -1)
              result_reg(regs) = __sys_ni_syscall(regs);
      instrumentation_end();

      syscall_exit_to_user_mode(regs);
}
```

不过仅依此并不能找到系统调用入口，因为并不知道这段代码在哪个文件

而且文档中写到`A typical syscall handling function invoked from low-level assembly code looks like this:`

也就是说，这段代码在系统调用中并不是完全长这样，至此线索就断开了

不过很快就想到，上文提到syscall将rip置成IA32_LSTAR寄存器中的值，即系统调用入口地址

因此想到可以通过动态分析让gdb输出IA32_LSTAR寄存器的值，所以接下来要跑内核

#### 跑内核

使用qemu将内核跑起来

1. kernel镜像

``` bash
make help
make defconfig
make nconfig / make menuconfig
make -j$(nproc) 2>&1 | tee build.log
```

> [!TIP]
> 注意，在配置内核的时候需要开启调试信息
> kernel hacking -> compile-time checks and compiler options -> Generate DWARF Version 5 debuginfo

编译成功结果

``` bash
  MKPIGGY arch/x86/boot/compressed/piggy.S
  AS      arch/x86/boot/compressed/piggy.o
  LD      arch/x86/boot/compressed/vmlinux
  ZOFFSET arch/x86/boot/zoffset.h
  OBJCOPY arch/x86/boot/vmlinux.bin
  AS      arch/x86/boot/header.o
  LD      arch/x86/boot/setup.elf
  OBJCOPY arch/x86/boot/setup.bin
  BUILD   arch/x86/boot/bzImage
Kernel: arch/x86/boot/bzImage is ready  (#1)
```

2. 临时文件系统镜像(initramfs)

`mkdir initramfs/{bin,proc,sys}`

---

安装busybox

``` bash
cd initramfs/bin/
curl -OL https://www.busybox.net/downloads/binaries/1.35.0-x86_64-linux-musl/busybox
chmod +x busybox
```

> [!TIP]
> [busybox](https://busybox.net/)非常神奇，类似于C语言中的联合体，其将很多必要命令和工具集成在一起

`for cmd in $(./busybox --list); do ln -s busybox $cmd; done` 创建符号连接到busybox，代替必要工具

---

创建启动脚本`touch init`，以下是脚本内容

``` bash
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
exec /bin/sh
```

别忘记给权限`chmod +x init`

---

镜像制作

``` bash
cd initramfs
find . | cpio -H newc -o | gzip > ../initramfs.cpio.gz
```

> [!TIP]
> cpio是归档工具

3. ~~原神，启动！~~

`qemu-system-x86_64 -kernel arch/x86/boot/bzImage -initrd initramfs.cpio.gz -append "nokaslr console=ttyS0" -serial mon:stdio -s`启动内核

> [!TIP]
> VNC是远程控制工具，不过这里用不到，是用qemu足够

`gdb -q vmlinux`启动gdb

``` gdb
$ gdb -q vmlinux
Reading symbols from vmlinux...
(gdb) target remote :1234  # 连接到qemu gdb服务，1234是人家默认端口，别瞎乱改
0x000000000000fff0 in exception_stacks ()
(gdb) 
```

#### gdb调试

内核跑起来了，结果发现由于IA32_LSTAR属于特定模式的寄存器，`info r`查不到

因此想通过IA32_LSTAR存放的系统调用入口地址来进入内核就失败了

万般无奈，因此突发奇想，之前的查看内核文档时，其中介绍了系统调用的大致样子，因此在gdb中对那些函数打断点

这样就可以通过backtrace反推出来入口地址

> [!TIP]
> 其实完全是碰运气，很有可能那些函数名都不在内核中，当然，除了这种方式其实还有很多种方式找到内核入口
> 但是本人就执着于不依赖网络，仅凭文档和gdb找到系统调用入口，因此才有这种做法

接着通过尝试，成功将断点打到了`syscall_enter_from_user_mode`函数上，由此查看backtrace如下

``` gdb
(gdb) b syscall_enter_from_user_mode
Breakpoint 1 at 0xffffffff81f7e1c6: file ./arch/x86/include/asm/irqflags.h, line 26.
(gdb) c
Continuing.

Breakpoint 1, syscall_enter_from_user_mode (regs=0xffffc90000013f58, syscall=0)
    at ./include/linux/entry-common.h:194
194		enter_from_user_mode(regs);
(gdb) backtrace
#0  syscall_enter_from_user_mode (regs=0xffffc90000013f58, syscall=0)
    at ./include/linux/entry-common.h:194
#1  do_syscall_64 (regs=0xffffc90000013f58, nr=0) at arch/x86/entry/common.c:79
#2  0xffffffff82000130 in entry_SYSCALL_64 () at arch/x86/entry/entry_64.S:121
#3  0x00007ffd1c9ed679 in ?? ()
#4  0x0000000000000000 in ?? ()
(gdb) 
```

所以，很容易就发现了名为`entry_SYSCALL_64`这个函数，仅仅看名字大概就能猜出来这就是苦苦寻找的系统调用入口

> [!TIP]
> 这里其实跳过了中断处理，但并不是讨论重点，因此掠过

### 入口汇编

到了这里，可以暂时把gdb放下，可以直接去看内核代码

查看arch/x86/entry/entry_64.S文件，以下是文件开头的注释

``` asm
// NOTE: arch/x86/entry/entry_64.S

.code64
.section .entry.text, "ax"

/*
 * 64-bit SYSCALL instruction entry. Up to 6 arguments in registers. <-- 64位内核调用入口，最多传入6个参数给内核
 *
 * This is the only entry point used for 64-bit system calls.  The
 * hardware interface is reasonably well designed and the register to  <-- 乱码七糟看不懂
 * argument mapping Linux uses fits well with the registers that are
 * available when SYSCALL is used.
 *
 * SYSCALL instructions can be found inlined in libc implementations as
 * well as some other programs and libraries.  There are also a handful  <-- 说什么SYSCALL在libc中也有啥的
 * of SYSCALL instructions in the vDSO used, for example, as a
 * clock_gettimeofday fallback.
 *
 * 64-bit SYSCALL saves rip to rcx, clears rflags.RF, then saves rflags to r11,
 * then loads new ss, cs, and rip from previously programmed MSRs.                <-- 正戏在这里
 * rflags gets masked by a value from another MSR (so CLD and CLAC
 * are not needed). SYSCALL does not save anything on the stack
 * and does not change rsp.
 *
 * Registers on entry:
 * rax  system call number
 * rcx  return address
 * r11  saved rflags (note: r11 is callee-clobbered register in C ABI)
 * rdi  arg0
 * rsi  arg1
 * rdx  arg2
 * r10  arg3 (needs to be moved to rcx to conform to C ABI)
 * r8   arg4
 * r9   arg5
 * (note: r12-r15, rbp, rbx are callee-preserved in C ABI)
 *
 * Only called from user space.
 *
 * When user can change pt_regs->foo always force IRET. That is because
 * it deals with uncanonical addresses better. SYSRET has trouble
 * with them due to bugs in both AMD and Intel CPUs.
 */
```

第一段注释写的很清楚，entry_SYSCALL_64就是我要找的函数

第三段注释写的很清楚，64位系统调用保存rip到rcx，清空rflags.rf,接着保存rflags到r11，之后加载新的ss，cs和rip从之前的程序MSR中

rflags被另一个MSR的值覆盖(所以CLS和CLAC不被需要)，系统调用不会保存东西到栈上，也不会改变rsp

这就是入口汇编所干的全部事情，好吧，汇编什么的都忘光光了，赶紧看[参考资料](#参考资料)恶补一下

恶补之后看根据文档里的解释，看下面的汇编

> [!TIP]
> 观察代码之前，请关注这部分[文档](https://www.kernel.org/doc/html/latest/core-api/asm-annotations.html)

``` asm
// NOTE: arch/x86/entry/entry_64.S

SYM_CODE_START(entry_SYSCALL_64)
	UNWIND_HINT_ENTRY
	ENDBR

	swapgs
	/* tss.sp2 is scratch space. */
	movq	%rsp, PER_CPU_VAR(cpu_tss_rw + TSS_sp2)
	SWITCH_TO_KERNEL_CR3 scratch_reg=%rsp
	movq	PER_CPU_VAR(pcpu_hot + X86_top_of_stack), %rsp

SYM_INNER_LABEL(entry_SYSCALL_64_safe_stack, SYM_L_GLOBAL)
	ANNOTATE_NOENDBR

	/* Construct struct pt_regs on stack */
	pushq	$__USER_DS				/* pt_regs->ss */
	pushq	PER_CPU_VAR(cpu_tss_rw + TSS_sp2)	/* pt_regs->sp */
	pushq	%r11					/* pt_regs->flags */
	pushq	$__USER_CS				/* pt_regs->cs */
	pushq	%rcx					/* pt_regs->ip */
SYM_INNER_LABEL(entry_SYSCALL_64_after_hwframe, SYM_L_GLOBAL)
	pushq	%rax					/* pt_regs->orig_ax */

	PUSH_AND_CLEAR_REGS rax=$-ENOSYS

	/* IRQs are off. */ <-- 这里中断已经关闭，查AI说在SYM_CODE_START(entry_SYSCALL_64)时候就关了，为了防止系统调用被打断
	movq	%rsp, %rdi
	/* Sign extend the lower 32bit as syscall numbers are treated as int */
	movslq	%eax, %rsi

	/* clobbers %rax, make sure it is after saving the syscall nr */
	IBRS_ENTER
	UNTRAIN_RET
	CLEAR_BRANCH_HISTORY

	call	do_syscall_64		/* returns with IRQs disabled */

	/*
	 * Try to use SYSRET instead of IRET if we're returning to
	 * a completely clean 64-bit userspace context.  If we're not,
	 * go to the slow exit path.
	 * In the Xen PV case we must use iret anyway.
	 */

	ALTERNATIVE "testb %al, %al; jz swapgs_restore_regs_and_return_to_usermode", \
		"jmp swapgs_restore_regs_and_return_to_usermode", X86_FEATURE_XENPV

	/*
	 * We win! This label is here just for ease of understanding
	 * perf profiles. Nothing jumps here.
	 */
syscall_return_via_sysret:
	IBRS_EXIT
	POP_REGS pop_rdi=0

	/*
	 * Now all regs are restored except RSP and RDI.
	 * Save old stack pointer and switch to trampoline stack.
	 */
	movq	%rsp, %rdi
	movq	PER_CPU_VAR(cpu_tss_rw + TSS_sp0), %rsp
	UNWIND_HINT_END_OF_STACK

	pushq	RSP-RDI(%rdi)	/* RSP */
	pushq	(%rdi)		/* RDI */

	/*
	 * We are on the trampoline stack.  All regs except RDI are live.
	 * We can do future final exit work right here.
	 */
	STACKLEAK_ERASE_NOCLOBBER

	SWITCH_TO_USER_CR3_STACK scratch_reg=%rdi

	popq	%rdi
	popq	%rsp
SYM_INNER_LABEL(entry_SYSRETQ_unsafe_stack, SYM_L_GLOBAL)
	ANNOTATE_NOENDBR
	swapgs
	CLEAR_CPU_BUFFERS
	sysretq
SYM_INNER_LABEL(entry_SYSRETQ_end, SYM_L_GLOBAL)
	ANNOTATE_NOENDBR
	int3
SYM_CODE_END(entry_SYSCALL_64)
```
第一次看啥也看不懂，深吸一口气再看一次，就看到了很多注释

循着注释看，就看明白了，这里再次[回顾glibc对寄存器的设置](#阅读源码)，真相就大白了

在glibc中，六个参数分别放到了rdi，rsi，r10，rdx，r8，r9，而系统调用号放到了众所周知的rax中

在上面的汇编代码中，很容易就发现这段

``` asm
// NOTE: arch/x86/entry/entry_64.S

SYM_INNER_LABEL(entry_SYSCALL_64_safe_stack, SYM_L_GLOBAL)
	ANNOTATE_NOENDBR

	/* Construct struct pt_regs on stack */
	pushq	$__USER_DS				/* pt_regs->ss */
	pushq	PER_CPU_VAR(cpu_tss_rw + TSS_sp2)	/* pt_regs->sp */
	pushq	%r11					/* pt_regs->flags */
	pushq	$__USER_CS				/* pt_regs->cs */
	pushq	%rcx					/* pt_regs->ip */
SYM_INNER_LABEL(entry_SYSCALL_64_after_hwframe, SYM_L_GLOBAL)
```

其中注释写的很清楚，`/* Construct struct pt_regs on stack */`

如何这里还是看不懂，那就暂时先掠过

接着可以在以上的源码中也能很轻松找到 `call	do_syscall_64		/* returns with IRQs disabled */`

跳转代码发现do_syscall_64，对照`do_syscall_64`函数传参看汇编代码，很容易就知道注释中的`pt_regs`是什么东西了

其实就是do_syscall_64的一个结构体形参，上边的汇编代码只不过是手动汇编传参的写法而已

到此，光知道pt_regs是个结构体还不行，针对传参的内容，还需要看看结构体的内部构造

因此跳转查看其定义

``` C
// NOTE: arch/x86/include/asm/ptrace.h

struct pt_regs {
	/*
	 * C ABI says these regs are callee-preserved. They aren't saved on
	 * kernel entry unless syscall needs a complete, fully filled
	 * "struct pt_regs".
	 */
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
	unsigned long bp;
	unsigned long bx;

	/* These regs are callee-clobbered. Always saved on kernel entry. */
	unsigned long r11;
	unsigned long r10;
	unsigned long r9;
	unsigned long r8;
	unsigned long ax;
	unsigned long cx;
	unsigned long dx;
	unsigned long si;
	unsigned long di;

	/*
	 * orig_ax is used on entry for:
	 * - the syscall number (syscall, sysenter, int80)
	 * - error_code stored by the CPU on traps and exceptions
	 * - the interrupt number for device interrupts
	 *
	 * A FRED stack frame starts here:
	 *   1) It _always_ includes an error code;
	 *
	 *   2) The return frame for ERET[US] starts here, but
	 *      the content of orig_ax is ignored.
	 */
	unsigned long orig_ax;

	/* The IRETQ return frame starts here */
	unsigned long ip;

	union {
		/* CS selector */
		u16		cs;
		/* The extended 64-bit data slot containing CS */
		u64		csx;
		/* The FRED CS extension */
		struct fred_cs	fred_cs;
	};

	unsigned long flags;
	unsigned long sp;

	union {
		/* SS selector */
		u16		ss;
		/* The extended 64-bit data slot containing SS */
		u64		ssx;
		/* The FRED SS extension */
		struct fred_ss	fred_ss;
	};

	/*
	 * Top of stack on IDT systems, while FRED systems have extra fields
	 * defined above for storing exception related information, e.g. CR2 or
	 * DR6.
	 */
};
```

上边的注释写的很明白，`unsigned long orig_ax;`就是存放系统调用号的

到此入口汇编就结束了，接下来就是C语言部分

### 入口C函数

目前知道在`do_syscall_64`中nr就是系统调用号，`pt_regs`存放了一系列系统调用传参

> [!NOTE]
> 搞到这里有感而发，注释真的很重要，就对于我本人而言，要是没有文档和注释，这系统调用探索之路根本无法进行下去
> 正是有了这些注释才能让我这个新手小白勉强看懂内核的代码
> ps: 函数名也很重要啊，内核的函数名真的很友好，基本上都是顾名思义

``` C
// NOTE: arch/x86/entry/common.c

/* Returns true to return using SYSRET, or false to use IRET */
__visible noinstr bool do_syscall_64(struct pt_regs *regs, int nr)
{
	add_random_kstack_offset();
	nr = syscall_enter_from_user_mode(regs, nr);

	instrumentation_begin();

	if (!do_syscall_x64(regs, nr) && !do_syscall_x32(regs, nr) && nr != -1) {
		/* Invalid system call, but still a system call. */
		regs->ax = __x64_sys_ni_syscall(regs);
	}

	instrumentation_end();
	syscall_exit_to_user_mode(regs);

	/*
	 * Check that the register state is valid for using SYSRET to exit
	 * to userspace.  Otherwise use the slower but fully capable IRET
	 * exit path.
	 */

	/* XEN PV guests always use the IRET path */
	if (cpu_feature_enabled(X86_FEATURE_XENPV))
		return false;

	/* SYSRET requires RCX == RIP and R11 == EFLAGS */
	if (unlikely(regs->cx != regs->ip || regs->r11 != regs->flags))
		return false;

	/* CS and SS must match the values set in MSR_STAR */
	if (unlikely(regs->cs != __USER_CS || regs->ss != __USER_DS))
		return false;

	/*
	 * On Intel CPUs, SYSRET with non-canonical RCX/RIP will #GP
	 * in kernel space.  This essentially lets the user take over
	 * the kernel, since userspace controls RSP.
	 *
	 * TASK_SIZE_MAX covers all user-accessible addresses other than
	 * the deprecated vsyscall page.
	 */
	if (unlikely(regs->ip >= TASK_SIZE_MAX))
		return false;

	/*
	 * SYSRET cannot restore RF.  It can restore TF, but unlike IRET,
	 * restoring TF results in a trap from userspace immediately after
	 * SYSRET.
	 */
	if (unlikely(regs->flags & (X86_EFLAGS_RF | X86_EFLAGS_TF)))
		return false;

	/* Use SYSRET to exit to userspace */
	return true;
}
```

通过观察，依旧很容易就能发现这个函数

``` C
// NOTE: arch/x86/entry/syscall_64.c

/*
 * The sys_call_table[] is no longer used for system calls, but
 * kernel/trace/trace_syscalls.c still wants to know the system
 * call address.
 */
#define __SYSCALL(nr, sym) __x64_##sym,
const sys_call_ptr_t sys_call_table[] = {
#include <asm/syscalls_64.h>
};
#undef  __SYSCALL

#define __SYSCALL(nr, sym) case nr: return __x64_##sym(regs);
long x64_sys_call(const struct pt_regs *regs, unsigned int nr)
{
	switch (nr) {
	#include <asm/syscalls_64.h>
	default: return __x64_sys_ni_syscall(regs);
	}
};
```

> [!TIP]
> C语言就是这样子的，看似简单的语法，实际上能搞出来贼抽象难以理解的程序，把`#include`写在函数里都是基操罢了

`include`写在函数里的作用就是一个超大的宏，大到一个文件

asm/syscalls_64中就是很多系统调用的宏封装，大致如下

``` C
// NOTE: arch/x86/include/generated/asm/syscalls_64.h

__SYSCALL(0, sys_read)
__SYSCALL(1, sys_write)
__SYSCALL(2, sys_open)
__SYSCALL(3, sys_close)
...
```

> [!TIP]
> 这里使用ctags显然不行，因此使用clang配合lazyvim的快捷键gd跳转到指定文件，在此之前需使用bear为clang生成compile_commands.json
> bear -- make -j8

### 系统调用函数

现在已经非常接近真相了，只需要找到对应的系统调用函数`__x64_sys_clone`就行了

只不过这里ctags和clang就失效了，无法跳转到__x64_sys_clone，只能用gdb调调看看咋回事

``` gdb
(gdb) info b
Num     Type           Disp Enb Address            What
5       breakpoint     keep y   0xffffffff81085ed0 in __x64_sys_clone at kernel/fork.c:2911
```

发现目标函数在kernel/fork.c的2911行

``` C
// NOTE: kernel/fork.c

#ifdef __ARCH_WANT_SYS_CLONE
#ifdef CONFIG_CLONE_BACKWARDS
SYSCALL_DEFINE5(clone, unsigned long, clone_flags, unsigned long, newsp,
		 int __user *, parent_tidptr,
		 unsigned long, tls,
		 int __user *, child_tidptr)
#elif defined(CONFIG_CLONE_BACKWARDS2)
SYSCALL_DEFINE5(clone, unsigned long, newsp, unsigned long, clone_flags,
		 int __user *, parent_tidptr,
		 int __user *, child_tidptr,
		 unsigned long, tls)
#elif defined(CONFIG_CLONE_BACKWARDS3)
SYSCALL_DEFINE6(clone, unsigned long, clone_flags, unsigned long, newsp,
		int, stack_size,
		int __user *, parent_tidptr,
		int __user *, child_tidptr,
		unsigned long, tls)
#else
SYSCALL_DEFINE5(clone, unsigned long, clone_flags, unsigned long, newsp, // <-- 这就是第2911行
		 int __user *, parent_tidptr,
		 int __user *, child_tidptr,
		 unsigned long, tls)
#endif
{
	struct kernel_clone_args args = {
		.flags		= (lower_32_bits(clone_flags) & ~CSIGNAL),
		.pidfd		= parent_tidptr,
		.child_tid	= child_tidptr,
		.parent_tid	= parent_tidptr,
		.exit_signal	= (lower_32_bits(clone_flags) & CSIGNAL),
		.stack		= newsp,
		.tls		= tls,
	};

	return kernel_clone(&args);
}
#endif
```

查看SYSCALL_DEFINE5的定义就知道为什么ctags和clang无法进行跳转了，因为这是个拼接宏

``` C
// NOTE: include/linux/syscalls.h

#define SYSCALL_METADATA(sname, nb, ...)

static inline int is_syscall_trace_event(struct trace_event_call *tp_event)
{
	return 0;
}
#endif

#ifndef SYSCALL_DEFINE0
#define SYSCALL_DEFINE0(sname)					\
	SYSCALL_METADATA(_##sname, 0);				\
	asmlinkage long sys_##sname(void);			\
	ALLOW_ERROR_INJECTION(sys_##sname, ERRNO);		\
	asmlinkage long sys_##sname(void)
#endif /* SYSCALL_DEFINE0 */

#define SYSCALL_DEFINE1(name, ...) SYSCALL_DEFINEx(1, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE2(name, ...) SYSCALL_DEFINEx(2, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE3(name, ...) SYSCALL_DEFINEx(3, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE4(name, ...) SYSCALL_DEFINEx(4, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE5(name, ...) SYSCALL_DEFINEx(5, _##name, __VA_ARGS__)
#define SYSCALL_DEFINE6(name, ...) SYSCALL_DEFINEx(6, _##name, __VA_ARGS__)

#define SYSCALL_DEFINE_MAXARGS	6

#define SYSCALL_DEFINEx(x, sname, ...)				\
	SYSCALL_METADATA(sname, x, __VA_ARGS__)			\
	__SYSCALL_DEFINEx(x, sname, __VA_ARGS__)

#define __PROTECT(...) asmlinkage_protect(__VA_ARGS__)

// NOTE: arch/x86/include/asm/syscall_wrapper.h

#define __SYSCALL_DEFINEx(x, name, ...)					\
	static long __se_sys##name(__MAP(x,__SC_LONG,__VA_ARGS__));	\
	static inline long __do_sys##name(__MAP(x,__SC_DECL,__VA_ARGS__));\
	__X64_SYS_STUBx(x, name, __VA_ARGS__)				\
	__IA32_SYS_STUBx(x, name, __VA_ARGS__)				\
	static long __se_sys##name(__MAP(x,__SC_LONG,__VA_ARGS__))	\
	{								\
		long ret = __do_sys##name(__MAP(x,__SC_CAST,__VA_ARGS__));\
		__MAP(x,__SC_TEST,__VA_ARGS__);				\
		__PROTECT(x, ret,__MAP(x,__SC_ARGS,__VA_ARGS__));	\
		return ret;						\
	}								\
	static inline long __do_sys##name(__MAP(x,__SC_DECL,__VA_ARGS__))

// NOTE: arch/x86/include/asm/syscall_wrapper.h

#define __X64_SYS_STUBx(x, name, ...)					\
	__SYS_STUBx(x64, sys##name,					\
		    SC_X86_64_REGS_TO_ARGS(x, __VA_ARGS__))
```

话说回来，到了这里系统调用内核之旅就彻底结束了

因为已经找到系统调用内核函数`kernel_clone`了，这就是`fork`函数的内核实现

## 参考资料

- 文档
  * [microsoft learn的处理器架构](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/processor-architecture)
  - [X64指令文档](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html?wapkw=x86-64%20instruction%20set)
  - [内核文档](https://docs.kernel.org/)
- 博客
  * [某佬博客](https://asjet.dev/2024/07/syscall_irl/)
- 视频
  * [北邮Young老师的汇编扫盲](https://www.bilibili.com/video/BV1gC4y1b7y8?p=1)
- 网站
  - [linux源码探索网站](https://elixir.bootlin.com/linux/v6.11.2/source)
