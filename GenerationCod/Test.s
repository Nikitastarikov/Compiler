  .data

  n:
    .long 1

  .text

  .globl main
  .type main, @function

  # main - точка входа, требуемая C runtime
main:
  # Пролог
  pushq %rbp  # Запушили в стек базовый регистр(используется для хранения указателя на некоторый объект в памяти)
  movq %rsp, %rbp  # Поместить регистр вершины стека(rsp) в базовый регистр(rbp)

  # (int n)
  subq $16, %rsp # %rsp = %rsp - $16
  xorl %eax, %eax # %eax = 0
  movq n, -4(%rbp)

  # scanf("%d", &n);
  #xorl %eax, %eax # %eax = 0
  #movq $decimal_format, %rdi # Поместить decimal_format в индекс приёмника, 
  #в цепочечных операциях содержит указатель на текущий элемент-приёмник
  #leaq -4(%rbp), %rsi # %rsi = %rbp - 4 
  #call scanf # вызов функции

  # printf("%d", n);
  xorl %eax, %eax # %eax = 0
  movq $decimal_format_nl, %rdi # Поместить decimal_format_nl в индекс приёмника, 
  #в цепочечных операциях содержит указатель на текущий элемент-приёмник
  movq -4(%rbp), %rsi # %rsi = %rbp - 4 
  call printf # вызов функции
  
  # Возвращаемое значение
  movl $0, %eax
  
  # Эпилог
  leave
  ret
  
  .section .rodata

decimal_format:
  .string "%d"

decimal_format_nl:
  .string "%d\n"


#leal  1(%eax,%ebx,2),%ecx  /* %ecx = %eax + %ebx × 2 + 1 = 25        */