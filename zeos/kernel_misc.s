# 0 "kernel_misc.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "kernel_misc.S"




# 1 "include/asm.h" 1
# 6 "kernel_misc.S" 2

.globl set_msr; .type set_msr, @function; .align 0; set_msr:
    push %ebp
    movl %esp, %ebp #prologue

    movl 0x8(%ebp), %ecx #first param (msr address)
    movl 0xc(%ebp), %eax #second param (low)
    movl 0x10(%ebp), %edx #third param (high)
    wrmsr # msr[%ecx] = %edx : %eax

    popl %ebp #epilogue
    ret

.globl task_switch; .type task_switch, @function; .align 0; task_switch:
    pushl %ebp
    movl %esp, %ebp

    pushl %esi
    pushl %edi
    pushl %ebx

    pushl 8(%ebp) # First parameter (pointer to new task_union)
    call inner_task_switch

    popl %ebx # Junk parameter
    popl %ebx
    popl %edi
    popl %esi

    popl %ebp
    ret

.globl switch_stacks; .type switch_stacks, @function; .align 0; switch_stacks: # Two parameters at 4(%esp) and 8(%esp)

    movl 4(%esp), %esi # current()->kernel_esp = %esp (to %ebp pushed by inner_task_switch)
    movl %ebp, (%esi)
    movl 8(%esp), %esp # %esp = new()->kernel_esp

    popl %ebp # Go back to task_switch. Forget about inner_task_switch
    ret # Jump directly to task_switch after call

.globl ret_from_fork; .type ret_from_fork, @function; .align 0; ret_from_fork:
    push %ebp
    movl %esp, %ebp

    movl $0, %eax

    pop %ebp
    ret
