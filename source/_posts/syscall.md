---
title: 系统调用流程
sticky: 500
---

# 系统调用流程

想了解系统调用实现原理，因此有了下文

## 前期准备

### 工具

需要一堆工具，缺啥装啥吧，反正实验环境是在arch linux，装啥都方便

### 源码与可执行文件

#### 辅助代码

准备以下代码与相应可执行文件

> [!TIP]
> 要静态编译(-static)，不然等下在qemu中没有ld也没有glibc.so就傻了

``` C
// NOTE: fork.c

#include <unistd.h>

int main () {
  fork();
  return 0;
}
```

#### 内核

> [!TIP]
> 也可以不编译内核，如果本身就在使用linux的话，可以在boot目录下找到Linux的映像文件

[kernel仓库](https://github.com/torvalds/linux)(版本：6，6点几忘了)

1.编译内核

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

2. 创建临时文件系统目录结构(initramfs)

``` bash
mkdir initramfs
cd initramfs
mkdir bin proc sys
cd bin
mkdir -p example/
```

---

3. 安装busybox

``` bash
cd initramfs/bin/
curl -OL https://www.busybox.net/downloads/binaries/1.35.0-x86_64-linux-musl/busybox
chmod +x busybox
```

> [!TIP]
> [busybox](https://busybox.net/)非常神奇，类似于C语言中的联合体，其将很多必要命令和工具集成在一起

`for cmd in $(./busybox --list); do ln -s busybox $cmd; done` 创建符号连接到busybox，代替必要工具

别忘了将将辅助代码的可执行文件放到example中

---

4. 创建启动脚本`touch init`

以下是脚本内容

``` bash
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
exec /bin/sh
```

别忘记给权限`chmod +x init`

---

5. 临时文件系统镜像制作

``` bash
cd initramfs
find . | cpio -H newc -o | gzip > ../initramfs.cpio.gz
```

> [!TIP]
> cpio是归档工具

#### glibc

[glibc源码获取](https://sourceware.org/glibc/started.html)(版本：2.40)

## 库源码

### 封装函数

用gdb看调用栈

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
> 这里其实跳过了一大步，通过各种尝试，发现"猜测"和`gcc -E`两种方法推断宏比较靠谱

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

本人就掉到这个坑里，一直没注意到ifndef，直到偶遇一篇[博客](https://dumphex.github.io/2020/03/01/syscall/)才发现找错了

而真正的宏在头文件`#include <sysdep.h>`中

> [!TIP]
> "sysdeps/unix/sysv/linux/x86_64/sysdep.h"，这个文件引用可以在arch-fork.h开头的头文件引用中找到，有的系统调用文件间接包含此头文件

``` C
// NOTE: sysdeps/unix/sysv/linux/x86_64/sysdep.h

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

通过查询[Intel文档](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html?wapkw=x86-64%20instruction%20set)得知syscall指令的大概行为

> RCX := RIP; 保存rip
> RIP := IA32_LSTAR; 将rip设置为系统调用入口地址
> R11 := RFLAGS; 保存标志寄存器
> RFLAGS := RFLAGS AND NOT(IA32_FMASK); 清空标志寄存器的一些位
> CPL := 0; 设置保护模式为0，即内核模式

> [!TIP]
> 以上是伪代码，":="其实就是赋值的意思，是pascal语言的赋值写法

其中IA32_LSTAR是跳转关键，即系统调用内核的入口

IA32_LSTAR是MSR寄存器，即模式特定寄存器，专门用来存放系统调用入口地址，查阅Intel手册得知IA32_LSTAR的地址为c000_0082H

> [!TIP]
> Intel手册有有多种形式，查找MSR不要看合集，直接查找第四卷即可

总结用户态的所有操作就是:系统调用被glibc封装，在glibc本质上做的是宏封装内联汇编传参

## 内核剖析

### 内核入口

上文提到syscall将rip置成IA32_LSTAR寄存器中的值，即系统调用入口地址

由于MSR是特殊寄存器，无法直接使用gdb查看其值，可以通过gdb的call指令调用内核函数来实现

> [!TIP]
> 这里其实有很多方式可以查看系统调用入口地址
> 根据不同限制，比如不能上网查，不能查看内核源码等等
> 因此有以下方法
> 1. 上网直接查/问AI/问大佬/查博客
> 2. 先查手册得知IA32_LSTAR地址，再通过rdmsr查出入口(需要把rdmsr搞到bin中，要不然只能看本机的，就算rsmsr搞到bin中也得注意这个rdmsr是需要静态编译的)
> 3. 通过编写动态内核模块通过调用内核函数来实现
> 4. 查手册得知IA32_LSTAR地址，再通过gdb的call命令调用内核函数来实现
> 5. 通过上网查得知syscall_init初始化MSR，再通过gdb打断点找到入口
> 其实不止以上方式，条条大路通罗马罢了
> 但经过对比衡量，选择了通过call调用内核函数`native_read_msr`来实现
> `native_read_msr`定义在文件include/asm/msr.h中

### 动态分析

使用qemu将内核跑起来，并通过gdb进行远程调试

`qemu-system-x86_64 -kernel arch/x86/boot/bzImage -initrd initramfs.cpio.gz -append "nokaslr console=ttyS0" -serial mon:stdio -s -S`启动内核

- nokaslr 禁用内核地址空间分布随机化
- -s 是启动gdb远程调试
- -S 是从头启动

`gdb -q vmlinux`启动gdb找系统调用入口

``` gdb
$ gdb -q vmlinux
Reading symbols from vmlinux...
(gdb) target remote :1234  # 连接到qemu gdb服务，1234是人家默认端口，别瞎乱改
0x000000000000fff0 in exception_stacks ()
(gdb) call native_read_msr(0xc0000082)
$3 = 18446744071595622528
(gdb) b *18446744071595622528
Breakpoint 1 at 0xffffffff82000080: file arch/x86/entry/entry_64.S, line 89.
(gdb)
```

通过查询得知入口在arch/x86/entry/entry_64.S第89行

> [!TIP]
> 这里其实跳过了中断处理，但并不是讨论重点，因此略过

### 入口汇编

查看内核文档，直接搜syscall很容易找到[这里](https://docs.kernel.org/trace/events-msr.html)

文档上写的很清楚，系统调用入口代码从汇编代码开始，在建立特定架构的低级状态和堆栈框架后调用低级C代码

代码大致如下

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

接着查看arch/x86/entry/entry_64.S文件，以下是文件开头的注释

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

第一段注释写的很清楚，entry_SYSCALL_64就是入口函数

第三段注释写的也很清楚，64位系统调用保存rip到rcx，清空rflags.rf,接着保存rflags到r11，之后加载新的ss，cs和rip从之前的程序MSR中

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
(gdb) b __x64_sys_clone
Breakpoint 2 at 0xffffffff81085ed0: file kernel/fork.c, line 2911.
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
  * [某佬博客1](https://asjet.dev/2024/07/syscall_irl/)
  * [某佬博客2](https://dumphex.github.io/2020/03/01/syscall/)
- 视频
  * [北邮Young老师的汇编扫盲](https://www.bilibili.com/video/BV1gC4y1b7y8?p=1)
- 网站
  - [linux源码探索网站](https://elixir.bootlin.com/linux/v6.11.2/source)
