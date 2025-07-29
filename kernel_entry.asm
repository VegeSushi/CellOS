bits 32

section .multiboot
    align 4
    dd 0x1BADB002           ; Multiboot magic number
    dd 0x00000003           ; Flags: bit 0 and 1 set (request mem info & video info)
    dd -(0x1BADB002 + 0x00000003)  ; Checksum so magic+flags+checksum = 0

section .text
    global boot_entry
    extern kernel_main

boot_entry:
    call kernel_main     ; call the C kernel function
    cli
    hlt
    jmp $

section .note.GNU-stack