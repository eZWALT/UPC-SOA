/*
 * wrapper.S - implementation of systemcall wrappers (libc.h)
 */

#include <asm.h>
#include <segment.h>

#define SAVE_ALL \
      pushl %gs; \
      pushl %fs; \
      pushl %es; \
      pushl %ds; \
      pushl %eax; \
      pushl %ebp; \
      pushl %edi; \
      pushl %esi; \
      pushl %ebx; \
      pushl %ecx; \
      pushl %edx; \
      movl $__KERNEL_DS, %edx;    \
      movl %edx, %ds;           \
      movl %edx, %es;

#define RESTORE_ALL \ 
      popl %edx; \
      popl %ecx; \
      popl %ebx; \ 
      popl %esi; \
      popl %edi; \
      popl %ebp; \ 
      popl %eax; \
      popl %ds; \
      popl %es; \ 
      popl %fs; \ 
      popl %gs; 

#define EOI  \ 
movb $0x20, %al; \ 
outb %al, $0x20; 


ENTRY(write)
      pushl %ebp
      mov %esp, %ebp
      mov %edx, (%ebp, )

      mov $4, %eax; \
      sysenter; \

      mov %ebp, %esp; \
      popl %ebp; \
      ret; \ 