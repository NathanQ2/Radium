# Radium
A hobby programming language. 

# Example
```
// Define entrypoint
func main() {
    // Create 'x' on the stack
    let x = 10 + 5;
    
    // Call function
    myFunc();
       
    // Exit with return code 'x'
    exit(x);
    ret 0;
}

func myFunc() {
    ret 5;
}
```

```shell
./Radium test.ra
```
Generated Linux x86-64 Assembly:
```asm
global _start

_start:
    call main
main:
    ; let
    mov rbp, 10
    mov rdi, 5
    add rbp, rdi
    push rbp

    ; call myFunc
    call myFunc

    ; exit
    push QWORD [rsp+0]
    pop rbp
    mov rdi, rbp
    mov rax, 60
    syscall

    mov rax, 0
    ret

myFunc:
    mov rax, 5
    ret
```