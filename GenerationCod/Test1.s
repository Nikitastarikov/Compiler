#int main()
#{
#    int i = 3;
#    int j = 6;
#    int k = 5;
#    int mas[k] = {1, 2, 3, 4};
#    printf("%d + %d * %d = %d ", i, j, k, (k = i + j * k))
#    i = 0;
#    while (i < k)
#    {
#       printf("%mas[%d] = %d", i, mas[i]);
#    }
#    return 0;
#}


.text

    .global main
    .type main, @function

main:

    pushl %ebp
    movl %esp, %ebp

    # k = i + j * k
    #subl  $20, %esp

    xorl %eax, %eax

    #movl 4(%ebp), %eax
    movl j, %eax
    add i, %eax
    xorl %ebx, %ebx

    movl k, %ebx
    mull %ebx

    movl $2, %ebx
    idivl %ebx
    
    pushl %eax
    pushl %ebx
    pushl k
    pushl i
    pushl j
    pushl $printf_f
    call printf

    #i = 0
    movl $0, i
    pushl i
    pushl $print_i
    call printf
    

start_loop:
    
    movl i, %ecx
    movl mas(,%ecx,4), %eax

    pushl %eax
    pushl i
    pushl $print_m
    call printf
    incl i
    #movl $4, %edx
    #movl i, %ecx

    cmpl $4, i
    jl start_loop

    movl %ebp, %esp
    popl %ebp  

    ret

.data
    printf_f:
        .string "(%d + %d) * %d / %d = %d \n"

    print_i:
        .string "i = %d\n"

    print_m:
        .string "mas[%d] = %d \n"

    i:
        .long 3
    j:
        .long 6
    k:
        .long 5
    mas:
        .long 1, 2, 3, 4