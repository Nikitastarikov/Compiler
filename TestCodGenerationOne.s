	.text

	.global main
	.type main, @function
main:
	pushl %ebp
	movl %esp, %ebp
	xorl %eax, %eax
	xorl %ebx, %ebx

	movl i, %eax
	movl $3, %ebx
	movl %ebx, i

	pushl %eax
	xorl %eax, %eax
	xorl %ebx, %ebx

	movl $5, %eax
	movl i, %ebx
	cmp %ebx, %eax
	jng else_blockWhile_1_1_2

if_blockWhile_1_1_2:
	incl i

	pushl i
	pushl $format_l_n
	call printf

	movl $5, %eax
	movl i, %ebx
	cmp %ebx, %eax
	jng else_blockWhile_1_1_2

	jmp if_blockWhile_1_1_2
else_blockWhile_1_1_2:
	leave
	ret

.data
	format_l:
		.string "%d"

	format_l_n:
		.string "%d\n"

	i:
		.long 0
	Temp:
		.long 0
