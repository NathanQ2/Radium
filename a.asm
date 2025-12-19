global _start

_start:
    ; let
    mov rbp, 5
    push rbp

    ; let
    ; call myFunc
    call myFunc
    mov rdi, rax
    push rdi

    ; if
    push QWORD [rsp+8]
    pop rsi
    mov rdx, 5
    sub rsi, rdx
    cmp rsi, 0
    jne .L1
    ; begin true
    ; if
    mov rdx, 10
    mov rsp, 1
    sub rdx, rsp
    cmp rdx, 0
    jne .L2
    ; begin true
    ; exit
    mov rax, 60
    mov rdi, 10
    syscall

    ; end true
.L2: 

    ; exit
    mov rax, 60
    mov rdi, 5
    syscall

    ; end true
.L1: 

    ; exit
    mov rax, 60
    mov rdi, 0
    syscall

myFunc:
    ; ret
    mov rsp, 15
    mov rcx, 15
    add rsp, rcx
    mov rax, rsp
    ret

