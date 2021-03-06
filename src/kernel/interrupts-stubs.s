.set IRQ_BASE, 0x20

.section .text

.extern _ZN8morphios6kernel17interruptsHandler15HandleInterruptEhj

.macro HandlerException num
.global _ZN8morphios6kernel17interruptsHandler20HandlerException\num\()Ev
_ZN8morphios6kernel17interruptsHandler20HandlerException\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro HandlerIRQ num
.global _ZN8morphios6kernel17interruptsHandler14HandlerIRQ\num\()Ev
_ZN8morphios6kernel17interruptsHandler14HandlerIRQ\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    pushl $0 # Filler for the error variable in CPUStruct
    jmp int_bottom
.endm


HandlerException 0x00
HandlerException 0x01
HandlerException 0x02
HandlerException 0x03
HandlerException 0x04
HandlerException 0x05
HandlerException 0x06
HandlerException 0x07
HandlerException 0x08
HandlerException 0x09
HandlerException 0x0A
HandlerException 0x0B
HandlerException 0x0C
HandlerException 0x0D
HandlerException 0x0E
HandlerException 0x0F
HandlerException 0x10
HandlerException 0x11
HandlerException 0x12
HandlerException 0x13

HandlerIRQ 0x00
HandlerIRQ 0x01
HandlerIRQ 0x02
HandlerIRQ 0x03
HandlerIRQ 0x04
HandlerIRQ 0x05
HandlerIRQ 0x06
HandlerIRQ 0x07
HandlerIRQ 0x08
HandlerIRQ 0x09
HandlerIRQ 0x0A
HandlerIRQ 0x0B
HandlerIRQ 0x0C
HandlerIRQ 0x0D
HandlerIRQ 0x0E
HandlerIRQ 0x0F
HandlerIRQ 0x31

# To preserve the stack during ISR execution
int_bottom:
    # Save registers
    # pusha
    # pushl %ds
    # pushl %es
    # pushl %fs
    # pushl %gs

    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    # Load ring 0 segment register
    # cld
    # mov $0x10, %eax
    # mov %eax, %eds
    # mov %eax, %ees

    # call C++ handler
    pushl %esp
    push (interruptnumber)
    call _ZN8morphios6kernel17interruptsHandler15HandleInterruptEhj
    # add %esp, 6
    mov %eax, %esp # switch the stack

    # Restore the saved registers
    popl %eax
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp

    # pop %gs
    # pop %fs
    # pop %es
    # pop %ds
    # popa
    add $4, %esp
    
    iret

.data
    interruptnumber: .byte 0
