//Шаблон

.text
  .globl main
  .type main, @function

  # main - точка входа, требуемая C runtime
main:
  # Пролог
  pushq %rbp
  movq %rsp, %rbp

  # Здесь будет основной код
  
  # Возвращаемое значение
  movl $0, %eax
  
  # Эпилог
  leave
  ret
