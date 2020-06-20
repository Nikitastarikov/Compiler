	.text

	.global main
	.type main, @function
main:
	pushl %ebp
	movl %esp, %ebp
	xorl %eax, %eax
	xorl %ebx, %ebx
	movl i, %eax
	movl $0, %ebx
	movl %ebx,i
	xorl %eax, %eax
	xorl %ebx, %ebx
	movl mas+0, %eax
	movl $5, %ebx
	movl %ebx,mas+0

	pushl i
	pushl $format_l_n
	call printf


	movl i, %ecx
	movl mas(, %ecx, 4), %eax
	pushl %eax
	pushl $format_l_n
	call printf

	xorl %eax, %eax
	xorl %ebx, %ebx
	movl i, %eax
	movl $1, %ebx
	movl %ebx,i
	xorl %eax, %eax
	xorl %ebx, %ebx
	movl mas+4, %eax
	movl $2, %ebx
	movl %ebx,mas+4

	pushl i
	pushl $format_l_n
	call printf


	movl i, %ecx
	movl mas(, %ecx, 4), %eax
	pushl %eax
	pushl $format_l_n
	call printf

	movl i, %ecx
	movl mas(, %ecx, 4), %eax
	decl %eax
	movl i, %ecx
	movl %eax, mas(, %ecx, 4)

	movl i, %ecx
	movl mas(, %ecx, 4), %eax
	pushl %eax
	pushl $format_l_n
	call printf

	xorl %eax, %eax
	xorl %ebx, %ebx
	movl mas+8, %eax
	movl $2, %ebx
	movl %ebx,mas+8
	xorl %eax, %eax
	xorl %ebx, %ebx
	movl i, %eax
	movl $0, %ebx
	movl %ebx,i
	xorl %eax, %eax
	xorl %ebx, %ebx

	xorl %eax, %eax
	xorl %ebx, %ebx
	movl i, %eax
	movl i, %ecx
	movl mas(, %ecx, 4), %ebx
	cmp %ebx, %eax
	jg else_blockWhile_1_1_14
if_blockWhile_1_1_14:
	xorl %eax, %eax
	xorl %ebx, %ebx
	movl $5, %eax
	movl i, %ebx
	addl %ebx, %eax
	movl %eax, k


	pushl k
	pushl $format_l_n
	call printf

	incl i
	xorl %eax, %eax
	xorl %ebx, %ebx
	movl i, %eax
	movl i, %ecx
	movl mas(, %ecx, 4), %ebx
	cmp %ebx, %eax
	jg else_blockWhile_1_1_14
	jmp if_blockWhile_1_1_14
else_blockWhile_1_1_14:
	leave
	ret

.data
	format_l:
		.string "%d"

	format_l_n:
		.string "%d\n"

	rio:
		.long 0
	i:
		.long 0
	mas:
		.long 0, 0, 0, 0, 0, 0
	k:
		.long 0
