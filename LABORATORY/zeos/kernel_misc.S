/*
 * kernel_misc.S - Functions that is useful for the kernel (Such as set MSR registers)
 */

#include <asm.h>

ENTRY(set_msr)
      push %ebp 
      movl %esp, %ebp               #prologue

      movl 0x8(%ebp), %ecx          #first param (msr address)
      movl 0xc(%ebp), %eax          #second param (low)
      movl 0x10(%ebp), %edx         #third param (high)
      wrmsr                         # msr[%ecx] = %edx : %eax 

      popl %ebp                     #epilogue
      ret 