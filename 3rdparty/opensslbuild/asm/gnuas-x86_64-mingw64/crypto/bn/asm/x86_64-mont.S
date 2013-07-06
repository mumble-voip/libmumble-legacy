.text	

.globl	bn_mul_mont
.def	bn_mul_mont;	.scl 2;	.type 32;	.endef
.p2align	4
bn_mul_mont:
	movq	%rdi,8(%rsp)
	movq	%rsi,16(%rsp)
	movq	%rsp,%rax
.LSEH_begin_bn_mul_mont:
	movq	%rcx,%rdi
	movq	%rdx,%rsi
	movq	%r8,%rdx
	movq	%r9,%rcx
	movq	40(%rsp),%r8
	movq	48(%rsp),%r9

	pushq	%rbx
	pushq	%rbp
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15

	movl	%r9d,%r9d
	leaq	2(%r9),%r10
	movq	%rsp,%r11
	negq	%r10
	leaq	(%rsp,%r10,8),%rsp
	andq	$-1024,%rsp

	movq	%r11,8(%rsp,%r9,8)
.Lprologue:
	movq	%rdx,%r12

	movq	(%r8),%r8

	xorq	%r14,%r14
	xorq	%r15,%r15

	movq	(%r12),%rbx
	movq	(%rsi),%rax
	mulq	%rbx
	movq	%rax,%r10
	movq	%rdx,%r11

	imulq	%r8,%rax
	movq	%rax,%rbp

	mulq	(%rcx)
	addq	%r10,%rax
	adcq	$0,%rdx
	movq	%rdx,%r13

	leaq	1(%r15),%r15
.L1st:
	movq	(%rsi,%r15,8),%rax
	mulq	%rbx
	addq	%r11,%rax
	adcq	$0,%rdx
	movq	%rax,%r10
	movq	(%rcx,%r15,8),%rax
	movq	%rdx,%r11

	mulq	%rbp
	addq	%r13,%rax
	leaq	1(%r15),%r15
	adcq	$0,%rdx
	addq	%r10,%rax
	adcq	$0,%rdx
	movq	%rax,-16(%rsp,%r15,8)
	cmpq	%r9,%r15
	movq	%rdx,%r13
	jl	.L1st

	xorq	%rdx,%rdx
	addq	%r11,%r13
	adcq	$0,%rdx
	movq	%r13,-8(%rsp,%r9,8)
	movq	%rdx,(%rsp,%r9,8)

	leaq	1(%r14),%r14
.p2align	2
.Louter:
	xorq	%r15,%r15

	movq	(%r12,%r14,8),%rbx
	movq	(%rsi),%rax
	mulq	%rbx
	addq	(%rsp),%rax
	adcq	$0,%rdx
	movq	%rax,%r10
	movq	%rdx,%r11

	imulq	%r8,%rax
	movq	%rax,%rbp

	mulq	(%rcx,%r15,8)
	addq	%r10,%rax
	movq	8(%rsp),%r10
	adcq	$0,%rdx
	movq	%rdx,%r13

	leaq	1(%r15),%r15
.p2align	2
.Linner:
	movq	(%rsi,%r15,8),%rax
	mulq	%rbx
	addq	%r11,%rax
	adcq	$0,%rdx
	addq	%rax,%r10
	movq	(%rcx,%r15,8),%rax
	adcq	$0,%rdx
	movq	%rdx,%r11

	mulq	%rbp
	addq	%r13,%rax
	leaq	1(%r15),%r15
	adcq	$0,%rdx
	addq	%r10,%rax
	adcq	$0,%rdx
	movq	(%rsp,%r15,8),%r10
	cmpq	%r9,%r15
	movq	%rax,-16(%rsp,%r15,8)
	movq	%rdx,%r13
	jl	.Linner

	xorq	%rdx,%rdx
	addq	%r11,%r13
	adcq	$0,%rdx
	addq	%r10,%r13
	adcq	$0,%rdx
	movq	%r13,-8(%rsp,%r9,8)
	movq	%rdx,(%rsp,%r9,8)

	leaq	1(%r14),%r14
	cmpq	%r9,%r14
	jl	.Louter

	leaq	(%rsp),%rsi
	leaq	-1(%r9),%r15

	movq	(%rsi),%rax
	xorq	%r14,%r14
	jmp	.Lsub
.p2align	4
.Lsub:	sbbq	(%rcx,%r14,8),%rax
	movq	%rax,(%rdi,%r14,8)
	decq	%r15
	movq	8(%rsi,%r14,8),%rax
	leaq	1(%r14),%r14
	jge	.Lsub

	sbbq	$0,%rax
	andq	%rax,%rsi
	notq	%rax
	movq	%rdi,%rcx
	andq	%rax,%rcx
	leaq	-1(%r9),%r15
	orq	%rcx,%rsi
.p2align	4
.Lcopy:
	movq	(%rsi,%r15,8),%rax
	movq	%rax,(%rdi,%r15,8)
	movq	%r14,(%rsp,%r15,8)
	decq	%r15
	jge	.Lcopy

	movq	8(%rsp,%r9,8),%rsi
	movq	$1,%rax
	movq	(%rsi),%r15
	movq	8(%rsi),%r14
	movq	16(%rsi),%r13
	movq	24(%rsi),%r12
	movq	32(%rsi),%rbp
	movq	40(%rsi),%rbx
	leaq	48(%rsi),%rsp
.Lepilogue:
	movq	8(%rsp),%rdi
	movq	16(%rsp),%rsi
	.byte	0xf3,0xc3
.LSEH_end_bn_mul_mont:
.byte	77,111,110,116,103,111,109,101,114,121,32,77,117,108,116,105,112,108,105,99,97,116,105,111,110,32,102,111,114,32,120,56,54,95,54,52,44,32,67,82,89,80,84,79,71,65,77,83,32,98,121,32,60,97,112,112,114,111,64,111,112,101,110,115,115,108,46,111,114,103,62,0
.p2align	4

.def	se_handler;	.scl 3;	.type 32;	.endef
.p2align	4
se_handler:
	pushq	%rsi
	pushq	%rdi
	pushq	%rbx
	pushq	%rbp
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15
	pushfq
	subq	$64,%rsp

	movq	120(%r8),%rax
	movq	248(%r8),%rbx

	leaq	.Lprologue(%rip),%r10
	cmpq	%r10,%rbx
	jb	.Lin_prologue

	movq	152(%r8),%rax

	leaq	.Lepilogue(%rip),%r10
	cmpq	%r10,%rbx
	jae	.Lin_prologue

	movq	192(%r8),%r10
	movq	8(%rax,%r10,8),%rax
	leaq	48(%rax),%rax

	movq	-8(%rax),%rbx
	movq	-16(%rax),%rbp
	movq	-24(%rax),%r12
	movq	-32(%rax),%r13
	movq	-40(%rax),%r14
	movq	-48(%rax),%r15
	movq	%rbx,144(%r8)
	movq	%rbp,160(%r8)
	movq	%r12,216(%r8)
	movq	%r13,224(%r8)
	movq	%r14,232(%r8)
	movq	%r15,240(%r8)

.Lin_prologue:
	movq	8(%rax),%rdi
	movq	16(%rax),%rsi
	movq	%rax,152(%r8)
	movq	%rsi,168(%r8)
	movq	%rdi,176(%r8)

	movq	40(%r9),%rdi
	movq	%r8,%rsi
	movl	$154,%ecx
.long	0xa548f3fc		

	movq	%r9,%rsi
	xorq	%rcx,%rcx
	movq	8(%rsi),%rdx
	movq	0(%rsi),%r8
	movq	16(%rsi),%r9
	movq	40(%rsi),%r10
	leaq	56(%rsi),%r11
	leaq	24(%rsi),%r12
	movq	%r10,32(%rsp)
	movq	%r11,40(%rsp)
	movq	%r12,48(%rsp)
	movq	%rcx,56(%rsp)
	call	*__imp_RtlVirtualUnwind(%rip)

	movl	$1,%eax
	addq	$64,%rsp
	popfq
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbp
	popq	%rbx
	popq	%rdi
	popq	%rsi
	.byte	0xf3,0xc3


.section	.pdata
.p2align	2
.rva	.LSEH_begin_bn_mul_mont
.rva	.LSEH_end_bn_mul_mont
.rva	.LSEH_info_bn_mul_mont

.section	.xdata
.p2align	3
.LSEH_info_bn_mul_mont:
.byte	9,0,0,0
.rva	se_handler
