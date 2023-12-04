[bits 16] ;ассемблер будет работать в 16битном режиме
[org 0x7c00] ;адрес старта программы
pre_set:
mov ax, 0x0000 ;адрес сегментов
mov ds, ax ;запись указателя сегмента данных
mov ss, ax ; запись указателя сегмента стека
mov sp, pre_set+1024+512 ;установка указателя стека


lab_start: 
mov AH, 0x0e
mov SI, MyName
lodsb ; загрузка байта из ячеёки памяти в регистр al
int 0x10
call show_string ; Вывести Имя
call next_line
mov AH, 0x0e
mov SI, MyGroup ; Вывести Группу
call show_string

call input_loop_start
jmp $

input_loop_start: ; Вход в цикл
call next_line 
mov AH, 0x0e
mov SI, PreInputText ; Вывод информации о выводе
call show_string

input_loop: ; Основной цикл ввода
mov ah, 
int 0x16

cmp al, 0x2b ; при нажатии на + переход на метку plus_key
je plus_key

cmp al, 0x2d ; при нажатии на - переход на метку minus_key
je minus_key

cmp al, 0x2a ; при нажатии на * переход на метку mult_key
je mult_key

cmp al, 0x2f ; при нажатии на / переход на метку divis_key
je divis_key

cmp ah, 0x01
je end_loop

jmp input_loop

plus_key: ; выполнение при нажатии на +
mov AH, 0x0e
mov al, 0x2b
int 0x10
call next_line
mov AH, 0x0e
mov SI, PlusOp
call show_string
jmp input_loop_start

minus_key: ; выполнение при нажатии на -
mov AH, 0x0e
mov al, 0x2d
int 0x10
call next_line
mov AH, 0x0e
mov SI, MinusOp
call show_string
jmp input_loop_start

mult_key: ; выполнение при нажатии на *
mov AH, 0x0e
mov al, 0x2a
int 0x10
call next_line
mov AH, 0x0e
mov SI, MultiOp
call show_string
jmp input_loop_start

divis_key: ; выполнение при нажатии на /
mov AH, 0x0e
mov al, 0x2f
int 0x10
call next_line
mov AH, 0x0e
mov SI, DiviOp
call show_string
jmp input_loop_start

end_loop: ; Написать END перед выходом из цикла
mov AH, 0x0e
mov al, 0x45 ; E
int 0x10
mov al, 0x4e ; N
int 0x10
mov al, 0x44 ; D
int 0x10
call next_line
call next_line
ret



show_string: ; Написать строку из ячейки памяти
cmp al, 0
je done
jne continue
continue:
lodsb
int 0x10 
jmp show_string
done:
ret

next_line: ; Переход га новую строку
mov AH, 3
mov al, DH
add al, 1
mov AH, 0x02
mov DH, al
int 0x10
ret



MyName:
db "Tyurin Ilya", 0

MyGroup:
db "8V01", 0

PreInputText:
db "Input + , - , * or / >", 0

PlusOp:
db "23 + 15 = 38", 0

MinusOp:
db "23 - 15 = 8", 0

MultiOp:
db "23 * 15 = 345", 0

DiviOp:
db "23 / 15 = 1,5(3)", 0

times 510-($-$$) db 0 ;Вставляем нули до 510 байта
dw 0xaa55 ;байты для загрузчика