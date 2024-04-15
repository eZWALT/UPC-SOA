/*
 * user_misc.S - implementation of systemcall wrappers (libc.h) and other user related stuff
 */

#include <asm.h>

ENTRY(write)
    pushl %ebp
    movl %esp, %ebp

    # Saving ebx, esi and edi
    pushl %ebx
    pushl %esi
    pushl %edi

    # System call index (see systemcall table)
    movl $4, %eax

    # Parameter passing through registers
    movl 0x8(%ebp), %edx   # fd 
    movl 0xc(%ebp), %ecx   # buffer
    movl 0x10(%ebp), %ebx  # size -> edx 

    # Saving ecx, edx and the return eip
    push %ecx 
    push %edx 
    push $sysenter_return_4

    # Faking a dynamic link (sysenter doesn't perform that automatically)
    push %ebp
    mov %esp, %ebp 
    sysenter

sysenter_return_4:
    pop %ebp    # restore ebp
    pop %edx    # useless @sysenter_return value 
    pop %edx    # restore edx 
    pop %ecx    # restore ecx

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


ENTRY(gettime)
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
    push $sysenter_return_10

    # Faking a dynamic link (sysenter doesn't perform that automatically)
    push %ebp
    mov %esp, %ebp 
    sysenter

sysenter_return_10:
    pop %ebp    # restore ebp
    pop %edx    # useless @sysenter_return value 
    pop %edx    # restore edx 
    pop %ecx    # restore ecx

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

    popl %ebp
    ret

ENTRY(getpid)
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
    pushl $sysenter_return_4

    #Fuking a dynamic link (Sysenter doesn't perform that automatically)
    push %ebp
    movl %esp, %ebp 
    sysenter

ENTRY(fork)
    pushl %ebp 
    movl %esp , %ebp 

    #Save ABI REGS: ebx, esi, edi
    pushl %ebx 
    pushl %esi
    pushl %edi 

    #System call index 20 
    movl $0x02, %eax 

    #Saving ecx, edx and return eip (Sysenter overwrrites them)
    pushl %ecx 
    pushl %edx 
    pushl $sysenter_return_4

    #F*king a dynamic link (Sysenter doesn't perform that automatically)
    push %ebp
    movl %esp, %ebp 
    sysenter
