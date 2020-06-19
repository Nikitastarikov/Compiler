.data
input_prompt:
        .string "enter size (1-255): "
 
scanf_format:
        .string "%d"
 
printf_format:
        .string "%5d "
 
printf_newline:
        .string "\n"
 
size:
        .long 0
 
.text
.global main
main:
       # /* запросить у пользователя размер таблицы */
        pushl $input_prompt    # /* format                            */
        call  printf            #/* вызов printf                      */
 
       # /* считать размер таблицы в переменную size */
        pushl $size            # /* указатель на переменную size      */
        pushl $scanf_format   #  /* format                            */
        call  scanf             #/* вызов scanf                       */
 
        addl  $12, %esp        # /* выровнять стек одной командой сразу 
                               #    после двух функций                */
 
        movl  $0, %eax         # /* в регистре %ax команда mulb будет 
                               #    выдавать результат, но мы печатаем 
                               #    всё содержимое %eax, поэтому два 
                               #    старших байта %eax должны быть 
                               #    нулевыми                          */
 
        movl  $0, %ebx         # /* номер строки                      */
 
print_line:
        incl  %ebx            #  /* увеличить номер строки на 1       */
        cmpl  size, %ebx
        ja    print_line_end   # /* если номер строки больше 
                               #    запрошенного размера, завершить цикл
                                  #                                   */
 
        movl  $0, %ecx         # /* номер колонки                     */
 
print_num:
        incl  %ecx             # /* увеличить номер колонки на 1      */
        cmpl  size, %ecx
        ja    print_num_end    # /* если номер колонки больше 
                               #    запрошенного размера, завершить цикл
                                #                                     */
 
        movb  %bl, %al         # /* команда mulb ожидает второй 
                               #    операнд в %al                     */
        mulb  %cl              # /* вычислить %ax = %cl * %al         */
 
        pushl %ebx              #/* сохранить используемые регистры 
                                #   перед вызовом printf              */
        pushl %ecx
 
        pushl %eax             # /* данные для печати                 */
        pushl $printf_format   # /* format                            */
        call  printf           # /* вызов printf                      */
        addl  $8, %esp         # /* выровнять стек                    */
 
        popl  %ecx             # /* восстановить регистры             */
        popl  %ebx
 
        jmp   print_num        # /* перейти в начало цикла            */
print_num_end:
 
        pushl %ebx             # /* сохранить регистр                 */
 
        pushl $printf_newline  # /* напечатать символ новой строки    */
        call  printf
        addl  $4, %esp
 
        popl  %ebx             # /* восстановить регистр              */
 
        jmp   print_line       # /* перейти в начало цикла            */
print_line_end:
 
        movl  $0, %eax        #  /* завершить программу               */
        ret