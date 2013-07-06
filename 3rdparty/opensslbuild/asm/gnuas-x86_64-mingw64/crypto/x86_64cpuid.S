
.section	.ctors
	.p2align	3
	.quad	OPENSSL_cpuid_setup

.text	

.globl	OPENSSL_atomic_add
.def	OPENSSL_atomic_add;	.scl 2;	.type 32;	.endef
.p2align	4
OPENSSL_atomic_add:
	movl	(%rcx),%eax
.Lspin:	leaq	(%rdx,%rax,1),%r8
.byte	0xf0		
	cmpxchgl	%r8d,(%rcx)
	jne	.Lspin
	movl	%r8d,%eax
.byte	0x48,0x98	
	.byte	0xf3,0xc3


.globl	OPENSSL_rdtsc
.def	OPENSSL_rdtsc;	.scl 2;	.type 32;	.endef
.p2align	4
OPENSSL_rdtsc:
	rdtsc
	shlq	$32,%rdx
	orq	%rdx,%rax
	.byte	0xf3,0xc3


.globl	OPENSSL_ia32_cpuid
.def	OPENSSL_ia32_cpuid;	.scl 2;	.type 32;	.endef
.p2align	4
OPENSSL_ia32_cpuid:
	movq	%rbx,%r8

	xorl	%eax,%eax
	cpuid
	movl	%eax,%r11d

	xorl	%eax,%eax
	cmpl	$1970169159,%ebx
	setne	%al
	movl	%eax,%r9d
	cmpl	$1231384169,%edx
	setne	%al
	orl	%eax,%r9d
	cmpl	$1818588270,%ecx
	setne	%al
	orl	%eax,%r9d
	jz	.Lintel

	cmpl	$1752462657,%ebx
	setne	%al
	movl	%eax,%r10d
	cmpl	$1769238117,%edx
	setne	%al
	orl	%eax,%r10d
	cmpl	$1145913699,%ecx
	setne	%al
	orl	%eax,%r10d
	jnz	.Lintel


	movl	$2147483648,%eax
	cpuid
	cmpl	$2147483656,%eax
	jb	.Lintel

	movl	$2147483656,%eax
	cpuid
	movzbq	%cl,%r10
	incq	%r10

	movl	$1,%eax
	cpuid
	btl	$28,%edx
	jnc	.Ldone
	shrl	$16,%ebx
	cmpb	%r10b,%bl
	ja	.Ldone
	andl	$4026531839,%edx
	jmp	.Ldone

.Lintel:
	cmpl	$4,%r11d
	movl	$-1,%r10d
	jb	.Lnocacheinfo

	movl	$4,%eax
	movl	$0,%ecx
	cpuid
	movl	%eax,%r10d
	shrl	$14,%r10d
	andl	$4095,%r10d

.Lnocacheinfo:
	movl	$1,%eax
	cpuid
	cmpl	$0,%r9d
	jne	.Lnotintel
	orl	$1048576,%edx
	andb	$15,%ah
	cmpb	$15,%ah
	je	.Lnotintel
	orl	$1073741824,%edx
.Lnotintel:
	btl	$28,%edx
	jnc	.Ldone
	andl	$4026531839,%edx
	cmpl	$0,%r10d
	je	.Ldone

	orl	$268435456,%edx
	shrl	$16,%ebx
	cmpb	$1,%bl
	ja	.Ldone
	andl	$4026531839,%edx
.Ldone:
	shlq	$32,%rcx
	movl	%edx,%eax
	movq	%r8,%rbx
	orq	%rcx,%rax
	.byte	0xf3,0xc3


.globl	OPENSSL_cleanse
.def	OPENSSL_cleanse;	.scl 2;	.type 32;	.endef
.p2align	4
OPENSSL_cleanse:
	xorq	%rax,%rax
	cmpq	$15,%rdx
	jae	.Lot
	cmpq	$0,%rdx
	je	.Lret
.Little:
	movb	%al,(%rcx)
	subq	$1,%rdx
	leaq	1(%rcx),%rcx
	jnz	.Little
.Lret:
	.byte	0xf3,0xc3
.p2align	4
.Lot:
	testq	$7,%rcx
	jz	.Laligned
	movb	%al,(%rcx)
	leaq	-1(%rdx),%rdx
	leaq	1(%rcx),%rcx
	jmp	.Lot
.Laligned:
	movq	%rax,(%rcx)
	leaq	-8(%rdx),%rdx
	testq	$-8,%rdx
	leaq	8(%rcx),%rcx
	jnz	.Laligned
	cmpq	$0,%rdx
	jne	.Little
	.byte	0xf3,0xc3

.globl	OPENSSL_wipe_cpu
.def	OPENSSL_wipe_cpu;	.scl 2;	.type 32;	.endef
.p2align	4
OPENSSL_wipe_cpu:
	pxor	%xmm0,%xmm0
	pxor	%xmm1,%xmm1
	pxor	%xmm2,%xmm2
	pxor	%xmm3,%xmm3
	pxor	%xmm4,%xmm4
	pxor	%xmm5,%xmm5
	xorq	%rcx,%rcx
	xorq	%rdx,%rdx
	xorq	%r8,%r8
	xorq	%r9,%r9
	xorq	%r10,%r10
	xorq	%r11,%r11
	leaq	8(%rsp),%rax
	.byte	0xf3,0xc3

