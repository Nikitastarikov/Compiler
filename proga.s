	.text

	.global main
	.type main, @function
main:
	pushl %ebp
	movl %esp, %ebp
	xorl %eax, %eax
	xorl %ebx, %ebx

	movl j, %eax
	movl $5, %ebx
	movl %ebx,j
	push %eax
	xorl %eax, %eax
	xorl %ebx, %ebx

	xorl %eax, %eax
	xorl %ebx, %ebx

	xorl %eax, %eax
	xorl %ebx, %ebx

	movl $10, %eax
	movl $2, %ebx
	mull %ebx
	push %eax
	popl %eax
	movl $4, %ebx
	pushl %eax
	sub %ebx, %eax
	pushl %eax
	pushl %eax
	xorl %eax, %eax
	xorl %ebx, %ebx

	movl j, %eax
	movl $1, %ebx
	addl %ebx, %eax
	push %eax
	popl %eax
	popl %ebx

	cmp %ebx, %eax
	jnl else_blockWhile_1_1_2
if_blockWhile_1_1_2:

	pushl j
	pushl $format_l_n
	call printf

	incl j
	xorl %eax, %eax
	xorl %ebx, %ebx

	xorl %eax, %eax
	xorl %ebx, %ebx

	movl $10, %eax
	movl $2, %ebx
	mull %ebx
	push %eax
	popl %eax
	movl $4, %ebx
	pushl %eax
	sub %ebx, %eax
	pushl %eax
	pushl %eax
	xorl %eax, %eax
	xorl %ebx, %ebx

	movl j, %eax
	movl $1, %ebx
	addl %ebx, %eax
	push %eax
	popl %eax
	popl %ebx

	cmp %ebx, %eax
	jnl else_blockWhile_1_1_2
	jmp if_blockWhile_1_1_2
else_blockWhile_1_1_2:
	leave
	ret

.data
	format_l:
		.string "%d"

	format_l_n:
		.string "%d\n"

	rio:
		.long 0
	j:
		.long 0
