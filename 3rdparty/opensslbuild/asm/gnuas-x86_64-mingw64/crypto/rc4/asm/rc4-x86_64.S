.text	

.globl	RC4
.def	RC4;	.scl 2;	.type 32;	.endef
.p2align	4
RC4:
	movq	%rdi,8(%rsp)
	movq	%rsi,16(%rsp)
	movq	%rsp,%rax
.LSEH_begin_RC4:
	movq	%rcx,%rdi
	movq	%rdx,%rsi
	movq	%r8,%rdx
	movq	%r9,%rcx
	orq	%rsi,%rsi
	jne	.Lentry
	movq	8(%rsp),%rdi
	movq	16(%rsp),%rsi
	.byte	0xf3,0xc3
.Lentry:
	pushq	%rbx
	pushq	%r12
	pushq	%r13
.Lprologue:

	addq	$8,%rdi
	movl	-8(%rdi),%r8d
	movl	-4(%rdi),%r12d
	cmpl	$-1,256(%rdi)
	je	.LRC4_CHAR
	incb	%r8b
	movl	(%rdi,%r8,4),%r9d
	testq	$-8,%rsi
	jz	.Lloop1
	jmp	.Lloop8
.p2align	4
.Lloop8:
	addb	%r9b,%r12b
	movq	%r8,%r10
	movl	(%rdi,%r12,4),%r13d
	rorq	$8,%rax
	incb	%r10b
	movl	(%rdi,%r10,4),%r11d
	cmpq	%r10,%r12
	movl	%r9d,(%rdi,%r12,4)
	cmoveq	%r9,%r11
	movl	%r13d,(%rdi,%r8,4)
	addb	%r9b,%r13b
	movb	(%rdi,%r13,4),%al
	addb	%r11b,%r12b
	movq	%r10,%r8
	movl	(%rdi,%r12,4),%r13d
	rorq	$8,%rax
	incb	%r8b
	movl	(%rdi,%r8,4),%r9d
	cmpq	%r8,%r12
	movl	%r11d,(%rdi,%r12,4)
	cmoveq	%r11,%r9
	movl	%r13d,(%rdi,%r10,4)
	addb	%r11b,%r13b
	movb	(%rdi,%r13,4),%al
	addb	%r9b,%r12b
	movq	%r8,%r10
	movl	(%rdi,%r12,4),%r13d
	rorq	$8,%rax
	incb	%r10b
	movl	(%rdi,%r10,4),%r11d
	cmpq	%r10,%r12
	movl	%r9d,(%rdi,%r12,4)
	cmoveq	%r9,%r11
	movl	%r13d,(%rdi,%r8,4)
	addb	%r9b,%r13b
	movb	(%rdi,%r13,4),%al
	addb	%r11b,%r12b
	movq	%r10,%r8
	movl	(%rdi,%r12,4),%r13d
	rorq	$8,%rax
	incb	%r8b
	movl	(%rdi,%r8,4),%r9d
	cmpq	%r8,%r12
	movl	%r11d,(%rdi,%r12,4)
	cmoveq	%r11,%r9
	movl	%r13d,(%rdi,%r10,4)
	addb	%r11b,%r13b
	movb	(%rdi,%r13,4),%al
	addb	%r9b,%r12b
	movq	%r8,%r10
	movl	(%rdi,%r12,4),%r13d
	rorq	$8,%rax
	incb	%r10b
	movl	(%rdi,%r10,4),%r11d
	cmpq	%r10,%r12
	movl	%r9d,(%rdi,%r12,4)
	cmoveq	%r9,%r11
	movl	%r13d,(%rdi,%r8,4)
	addb	%r9b,%r13b
	movb	(%rdi,%r13,4),%al
	addb	%r11b,%r12b
	movq	%r10,%r8
	movl	(%rdi,%r12,4),%r13d
	rorq	$8,%rax
	incb	%r8b
	movl	(%rdi,%r8,4),%r9d
	cmpq	%r8,%r12
	movl	%r11d,(%rdi,%r12,4)
	cmoveq	%r11,%r9
	movl	%r13d,(%rdi,%r10,4)
	addb	%r11b,%r13b
	movb	(%rdi,%r13,4),%al
	addb	%r9b,%r12b
	movq	%r8,%r10
	movl	(%rdi,%r12,4),%r13d
	rorq	$8,%rax
	incb	%r10b
	movl	(%rdi,%r10,4),%r11d
	cmpq	%r10,%r12
	movl	%r9d,(%rdi,%r12,4)
	cmoveq	%r9,%r11
	movl	%r13d,(%rdi,%r8,4)
	addb	%r9b,%r13b
	movb	(%rdi,%r13,4),%al
	addb	%r11b,%r12b
	movq	%r10,%r8
	movl	(%rdi,%r12,4),%r13d
	rorq	$8,%rax
	incb	%r8b
	movl	(%rdi,%r8,4),%r9d
	cmpq	%r8,%r12
	movl	%r11d,(%rdi,%r12,4)
	cmoveq	%r11,%r9
	movl	%r13d,(%rdi,%r10,4)
	addb	%r11b,%r13b
	movb	(%rdi,%r13,4),%al
	rorq	$8,%rax
	subq	$8,%rsi

	xorq	(%rdx),%rax
	addq	$8,%rdx
	movq	%rax,(%rcx)
	addq	$8,%rcx

	testq	$-8,%rsi
	jnz	.Lloop8
	cmpq	$0,%rsi
	jne	.Lloop1
	jmp	.Lexit

.p2align	4
.Lloop1:
	addb	%r9b,%r12b
	movl	(%rdi,%r12,4),%r13d
	movl	%r9d,(%rdi,%r12,4)
	movl	%r13d,(%rdi,%r8,4)
	addb	%r13b,%r9b
	incb	%r8b
	movl	(%rdi,%r9,4),%r13d
	movl	(%rdi,%r8,4),%r9d
	xorb	(%rdx),%r13b
	incq	%rdx
	movb	%r13b,(%rcx)
	incq	%rcx
	decq	%rsi
	jnz	.Lloop1
	jmp	.Lexit

.p2align	4
.LRC4_CHAR:
	addb	$1,%r8b
	movzbl	(%rdi,%r8,1),%r9d
	testq	$-8,%rsi
	jz	.Lcloop1
	cmpl	$0,260(%rdi)
	jnz	.Lcloop1
	jmp	.Lcloop8
.p2align	4
.Lcloop8:
	movl	(%rdx),%eax
	movl	4(%rdx),%ebx
	addb	%r9b,%r12b
	leaq	1(%r8),%r10
	movzbl	(%rdi,%r12,1),%r13d
	movzbl	%r10b,%r10d
	movzbl	(%rdi,%r10,1),%r11d
	movb	%r9b,(%rdi,%r12,1)
	cmpq	%r10,%r12
	movb	%r13b,(%rdi,%r8,1)
	jne	.Lcmov0			
	movq	%r9,%r11
.Lcmov0:
	addb	%r9b,%r13b
	xorb	(%rdi,%r13,1),%al
	rorl	$8,%eax
	addb	%r11b,%r12b
	leaq	1(%r10),%r8
	movzbl	(%rdi,%r12,1),%r13d
	movzbl	%r8b,%r8d
	movzbl	(%rdi,%r8,1),%r9d
	movb	%r11b,(%rdi,%r12,1)
	cmpq	%r8,%r12
	movb	%r13b,(%rdi,%r10,1)
	jne	.Lcmov1			
	movq	%r11,%r9
.Lcmov1:
	addb	%r11b,%r13b
	xorb	(%rdi,%r13,1),%al
	rorl	$8,%eax
	addb	%r9b,%r12b
	leaq	1(%r8),%r10
	movzbl	(%rdi,%r12,1),%r13d
	movzbl	%r10b,%r10d
	movzbl	(%rdi,%r10,1),%r11d
	movb	%r9b,(%rdi,%r12,1)
	cmpq	%r10,%r12
	movb	%r13b,(%rdi,%r8,1)
	jne	.Lcmov2			
	movq	%r9,%r11
.Lcmov2:
	addb	%r9b,%r13b
	xorb	(%rdi,%r13,1),%al
	rorl	$8,%eax
	addb	%r11b,%r12b
	leaq	1(%r10),%r8
	movzbl	(%rdi,%r12,1),%r13d
	movzbl	%r8b,%r8d
	movzbl	(%rdi,%r8,1),%r9d
	movb	%r11b,(%rdi,%r12,1)
	cmpq	%r8,%r12
	movb	%r13b,(%rdi,%r10,1)
	jne	.Lcmov3			
	movq	%r11,%r9
.Lcmov3:
	addb	%r11b,%r13b
	xorb	(%rdi,%r13,1),%al
	rorl	$8,%eax
	addb	%r9b,%r12b
	leaq	1(%r8),%r10
	movzbl	(%rdi,%r12,1),%r13d
	movzbl	%r10b,%r10d
	movzbl	(%rdi,%r10,1),%r11d
	movb	%r9b,(%rdi,%r12,1)
	cmpq	%r10,%r12
	movb	%r13b,(%rdi,%r8,1)
	jne	.Lcmov4			
	movq	%r9,%r11
.Lcmov4:
	addb	%r9b,%r13b
	xorb	(%rdi,%r13,1),%bl
	rorl	$8,%ebx
	addb	%r11b,%r12b
	leaq	1(%r10),%r8
	movzbl	(%rdi,%r12,1),%r13d
	movzbl	%r8b,%r8d
	movzbl	(%rdi,%r8,1),%r9d
	movb	%r11b,(%rdi,%r12,1)
	cmpq	%r8,%r12
	movb	%r13b,(%rdi,%r10,1)
	jne	.Lcmov5			
	movq	%r11,%r9
.Lcmov5:
	addb	%r11b,%r13b
	xorb	(%rdi,%r13,1),%bl
	rorl	$8,%ebx
	addb	%r9b,%r12b
	leaq	1(%r8),%r10
	movzbl	(%rdi,%r12,1),%r13d
	movzbl	%r10b,%r10d
	movzbl	(%rdi,%r10,1),%r11d
	movb	%r9b,(%rdi,%r12,1)
	cmpq	%r10,%r12
	movb	%r13b,(%rdi,%r8,1)
	jne	.Lcmov6			
	movq	%r9,%r11
.Lcmov6:
	addb	%r9b,%r13b
	xorb	(%rdi,%r13,1),%bl
	rorl	$8,%ebx
	addb	%r11b,%r12b
	leaq	1(%r10),%r8
	movzbl	(%rdi,%r12,1),%r13d
	movzbl	%r8b,%r8d
	movzbl	(%rdi,%r8,1),%r9d
	movb	%r11b,(%rdi,%r12,1)
	cmpq	%r8,%r12
	movb	%r13b,(%rdi,%r10,1)
	jne	.Lcmov7			
	movq	%r11,%r9
.Lcmov7:
	addb	%r11b,%r13b
	xorb	(%rdi,%r13,1),%bl
	rorl	$8,%ebx
	leaq	-8(%rsi),%rsi
	movl	%eax,(%rcx)
	leaq	8(%rdx),%rdx
	movl	%ebx,4(%rcx)
	leaq	8(%rcx),%rcx

	testq	$-8,%rsi
	jnz	.Lcloop8
	cmpq	$0,%rsi
	jne	.Lcloop1
	jmp	.Lexit
.p2align	4
.Lcloop1:
	addb	%r9b,%r12b
	movzbl	(%rdi,%r12,1),%r13d
	movb	%r9b,(%rdi,%r12,1)
	movb	%r13b,(%rdi,%r8,1)
	addb	%r9b,%r13b
	addb	$1,%r8b
	movzbl	%r13b,%r13d
	movzbl	%r8b,%r8d
	movzbl	(%rdi,%r13,1),%r13d
	movzbl	(%rdi,%r8,1),%r9d
	xorb	(%rdx),%r13b
	leaq	1(%rdx),%rdx
	movb	%r13b,(%rcx)
	leaq	1(%rcx),%rcx
	subq	$1,%rsi
	jnz	.Lcloop1
	jmp	.Lexit

.p2align	4
.Lexit:
	subb	$1,%r8b
	movl	%r8d,-8(%rdi)
	movl	%r12d,-4(%rdi)

	movq	(%rsp),%r13
	movq	8(%rsp),%r12
	movq	16(%rsp),%rbx
	addq	$24,%rsp
.Lepilogue:
	movq	8(%rsp),%rdi
	movq	16(%rsp),%rsi
	.byte	0xf3,0xc3
.LSEH_end_RC4:

.globl	RC4_set_key
.def	RC4_set_key;	.scl 2;	.type 32;	.endef
.p2align	4
RC4_set_key:
	movq	%rdi,8(%rsp)
	movq	%rsi,16(%rsp)
	movq	%rsp,%rax
.LSEH_begin_RC4_set_key:
	movq	%rcx,%rdi
	movq	%rdx,%rsi
	movq	%r8,%rdx

	leaq	8(%rdi),%rdi
	leaq	(%rdx,%rsi,1),%rdx
	negq	%rsi
	movq	%rsi,%rcx
	xorl	%eax,%eax
	xorq	%r9,%r9
	xorq	%r10,%r10
	xorq	%r11,%r11

	movl	OPENSSL_ia32cap_P(%rip),%r8d
	btl	$20,%r8d
	jnc	.Lw1stloop
	btl	$30,%r8d
	setc	%r9b
	movl	%r9d,260(%rdi)
	jmp	.Lc1stloop

.p2align	4
.Lw1stloop:
	movl	%eax,(%rdi,%rax,4)
	addb	$1,%al
	jnc	.Lw1stloop

	xorq	%r9,%r9
	xorq	%r8,%r8
.p2align	4
.Lw2ndloop:
	movl	(%rdi,%r9,4),%r10d
	addb	(%rdx,%rsi,1),%r8b
	addb	%r10b,%r8b
	addq	$1,%rsi
	movl	(%rdi,%r8,4),%r11d
	cmovzq	%rcx,%rsi
	movl	%r10d,(%rdi,%r8,4)
	movl	%r11d,(%rdi,%r9,4)
	addb	$1,%r9b
	jnc	.Lw2ndloop
	jmp	.Lexit_key

.p2align	4
.Lc1stloop:
	movb	%al,(%rdi,%rax,1)
	addb	$1,%al
	jnc	.Lc1stloop

	xorq	%r9,%r9
	xorq	%r8,%r8
.p2align	4
.Lc2ndloop:
	movb	(%rdi,%r9,1),%r10b
	addb	(%rdx,%rsi,1),%r8b
	addb	%r10b,%r8b
	addq	$1,%rsi
	movb	(%rdi,%r8,1),%r11b
	jnz	.Lcnowrap
	movq	%rcx,%rsi
.Lcnowrap:
	movb	%r10b,(%rdi,%r8,1)
	movb	%r11b,(%rdi,%r9,1)
	addb	$1,%r9b
	jnc	.Lc2ndloop
	movl	$-1,256(%rdi)

.p2align	4
.Lexit_key:
	xorl	%eax,%eax
	movl	%eax,-8(%rdi)
	movl	%eax,-4(%rdi)
	movq	8(%rsp),%rdi
	movq	16(%rsp),%rsi
	.byte	0xf3,0xc3
.LSEH_end_RC4_set_key:

.globl	RC4_options
.def	RC4_options;	.scl 2;	.type 32;	.endef
.p2align	4
RC4_options:
	leaq	.Lopts(%rip),%rax
	movl	OPENSSL_ia32cap_P(%rip),%edx
	btl	$20,%edx
	jnc	.Ldone
	addq	$12,%rax
	btl	$30,%edx
	jnc	.Ldone
	addq	$13,%rax
.Ldone:
	.byte	0xf3,0xc3
.p2align	6
.Lopts:
.byte	114,99,52,40,56,120,44,105,110,116,41,0
.byte	114,99,52,40,56,120,44,99,104,97,114,41,0
.byte	114,99,52,40,49,120,44,99,104,97,114,41,0
.byte	82,67,52,32,102,111,114,32,120,56,54,95,54,52,44,32,67,82,89,80,84,79,71,65,77,83,32,98,121,32,60,97,112,112,114,111,64,111,112,101,110,115,115,108,46,111,114,103,62,0
.p2align	6


.def	stream_se_handler;	.scl 3;	.type 32;	.endef
.p2align	4
stream_se_handler:
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

	leaq	24(%rax),%rax

	movq	-8(%rax),%rbx
	movq	-16(%rax),%r12
	movq	-24(%rax),%r13
	movq	%rbx,144(%r8)
	movq	%r12,216(%r8)
	movq	%r13,224(%r8)

.Lin_prologue:
	movq	8(%rax),%rdi
	movq	16(%rax),%rsi
	movq	%rax,152(%r8)
	movq	%rsi,168(%r8)
	movq	%rdi,176(%r8)

	jmp	.Lcommon_seh_exit


.def	key_se_handler;	.scl 3;	.type 32;	.endef
.p2align	4
key_se_handler:
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

	movq	152(%r8),%rax
	movq	8(%rax),%rdi
	movq	16(%rax),%rsi
	movq	%rsi,168(%r8)
	movq	%rdi,176(%r8)

.Lcommon_seh_exit:

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
.rva	.LSEH_begin_RC4
.rva	.LSEH_end_RC4
.rva	.LSEH_info_RC4

.rva	.LSEH_begin_RC4_set_key
.rva	.LSEH_end_RC4_set_key
.rva	.LSEH_info_RC4_set_key

.section	.xdata
.p2align	3
.LSEH_info_RC4:
.byte	9,0,0,0
.rva	stream_se_handler
.LSEH_info_RC4_set_key:
.byte	9,0,0,0
.rva	key_se_handler
