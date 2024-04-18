# 0 "user_misc.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "user_misc.S"




# 1 "include/asm.h" 1
# 6 "user_misc.S" 2

.globl write; .type write, @function; .align 0; write:
    pushl %ebp
    movl %esp, %ebp

    # Saving ebx, esi and edi
    pushl %ebx
    pushl %esi
    pushl %edi

    # System call index (see systemcall table)
    movl $4, %eax

    # Parameter passing through registers
    movl 0x8(%ebp), %edx # fd
    movl 0xc(%ebp), %ecx # buffer
    movl 0x10(%ebp), %ebx # size -> edx

    # Saving ecx, edx and the return eip
    push %ecx
    push %edx
    push $sysenter_return

    # Faking a dynamic link (sysenter doesn't perform that automatically)
    push %ebp
    mov %esp, %ebp
    sysenter

sysenter_return:
    pop %ebp # restore ebp
    pop %edx # useless @sysenter_return value
    pop %edx # restore edx
    pop %ecx # restore ecx

    popl %edi
    popl %esi
    popl %ebx

    # If eax is positive, jump to the end
    test %eax, %eax
    jns fin

    # Error
    neg %eax
    mov %eax, errno
    mov $-1, %eax

fin:
    popl %ebp
    ret


.globl gettime; .type gettime, @function; .align 0; gettime:
    pushl %ebp
    movl %esp, %ebp

    # Saving ebx, esi and edi
    pushl %ebx
    pushl %esi
    pushl %edi

    # System call index (see systemcall table)
    movl $10, %eax

    # Saving ecx, edx and the return eip
    push %ecx
    push %edx
    push $sysenter_return

    # Faking a dynamic link (sysenter doesn't perform that automatically)
    push %ebp
    mov %esp, %ebp
    sysenter

.globl getpid; .type getpid, @function; .align 0; getpid:
    pushl %ebp
    movl %esp , %ebp

    #Save ABI REGS: ebx, esi, edi
    pushl %ebx
    pushl %esi
    pushl %edi

    #System call index 20
    movl $20, %eax

    #Saving ecx, edx and return eip (Sysenter overwrrites them)
    pushl %ecx
    pushl %edx
    pushl $sysenter_return

    #Fuking a dynamic link (Sysenter doesn't perform that automatically)
    push %ebp
    movl %esp, %ebp
    sysenter

.globl fork; .type fork, @function; .align 0; fork:
    pushl %ebp
    movl %esp , %ebp

    #Save ABI REGS: ebx, esi, edi
    pushl %ebx
    pushl %esi
    pushl %edi

    #System call index 2
    movl $0x02, %eax

    #Saving ecx, edx and return eip (Sysenter overwrrites them)
    pushl %ecx
    pushl %edx
    pushl $sysenter_return

    #F*king a dynamic link (Sysenter doesn't perform that automatically)
    push %ebp
    movl %esp, %ebp
    sysenter

.globl exit; .type exit, @function; .align 0; exit:
    pushl %ebp
    movl %esp, %ebp

    pushl %ebx
    pushl %esi
    pushl %edi

    movl $0x01, %eax

    pushl %ecx
    pushl %edx
    pushl $sysenter_return

    push %ebp
    movl %esp, %ebp
    sysenter

.globl block; .type block, @function; .align 0; block:
    pushl %ebp
    movl %esp, %ebp

    pushl %ebx
    pushl %esi
    pushl %edi

    movl $0x12, %eax

    pushl %ecx
    pushl %edx
    pushl $sysenter_return

    push %ebp
    movl %esp, %ebp
    sysenter

.globl unblock; .type unblock, @function; .align 0; unblock:
    pushl %ebp
    movl %esp, %ebp

    pushl %ebx
    pushl %esi
    pushl %edi

    movl 0x8(%ebp), %edx # PID Parameter
    movl $0x13, %eax

    pushl %ecx
    pushl %edx
    pushl $sysenter_return

    push %ebp
    movl %esp, %ebp
    sysenter

.globl numsons; .type numsons, @function; .align 0; numsons:
    pushl %ebp
    movl %esp, %ebp

    pushl %ebx
    pushl %esi
    pushl %edi

    movl $0x15, %eax

    pushl %ecx
    pushl %edx
    pushl $sysenter_return

    push %ebp
    movl %esp, %ebp
    sysenter

.globl numbros; .type numbros, @function; .align 0; numbros:
    pushl %ebp
    movl %esp, %ebp

    pushl %ebx
    pushl %esi
    pushl %edi

    movl $0x16, %eax

    pushl %ecx
    pushl %edx
    pushl $sysenter_return

    push %ebp
    movl %esp, %ebp
    sysenter
