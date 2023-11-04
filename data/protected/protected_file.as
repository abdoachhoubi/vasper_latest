section .data
    msg db 'vasper', 0
    msglen equ $ - msg

section .text
global _start

_start:
    ; File descriptor for STDOUT (1)
    mov eax, 4
    mov ebx, 1
    mov ecx, msg
    mov edx, msglen
    int 0x80

    ; Exit program
    mov eax, 1
    int 0x80
