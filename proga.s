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
	xorl %eax, %eax
	xorl %ebx, %ebx
	movl $1, %eax
	movl $2, %ebx
	addl %ebx, %eax
	push %eax
	xorl %eax, %eax
	xorl %ebx, %ebx
	movl $5, %eax
	movl j, %ebx
	mull %ebx
	push %eax
	popl %eax
	popl %ebx
	pushl %eax
	idivl %ebx
	pushl %eax
	popl %ebx
	movl $5, %eax
	pushl %eax
	addl %ebx, %eax
	pushl %eax
	popl %ebx
	movl i, %eax
	pushl %eax
	movl %ebx,i
	pushl %eax

	pushl i
	pushl $format_l_n
	call printf

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
	i:
		.long 0
