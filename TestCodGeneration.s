	.text

	.global main
	.type main, @function
main:
	pushl %ebp
	movl %esp, %ebp
	xorl %eax, %eax
	xorl %ebx, %ebx

	xorl %eax, %eax
	xorl %ebx, %ebx

	movl $6, %eax
	movl $1, %ebx
	addl %ebx, %eax
	pushl %eax
	popl %ebx
	movl i, %eax
	pushl %eax
	movl %ebx,i
	pushl %eax
	leave
	ret

.data
	format_l:
		.string "%d"

	format_l_n:
		.string "%d\n"

	i:
		.long 0
