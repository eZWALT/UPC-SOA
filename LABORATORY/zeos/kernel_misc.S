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

ENTRY(task_switch)
      pushl %ebp 
      movl %esp, %ebp 

      //SAVE ABI REGS 
      pushl %esi 
      pushl %edi
      pushl %ebx 

      //PUSH NEW
      pushl 8(%ebp)
      call inner_task_switch

      popl %ebx 
      popl %edi 
      popl %esi 

      // Restaurem el stack pointer 
      popl %ebp 
      ret 

//2 parametres: (sp, new_sp)
ENTRY(switch_stacks)
      //Emagatzema %ebp en el cim del sp (Quan torni el proces la seva pila seguira desde aqui)
      movl %ebp, 4(%esp)
      //mou el valor del new_sp dins de %esp
      movl 8(%esp), %esp 
      // restaura %ebp i %eip  (Tornem a task_switch)
      popl %ebp 
      ret 

//2 parametres: (sp, new_sp)
ENTRY(ss2)
      pushl %ebp 
      movl %esp, %ebp 
      
      movl 8(%ebp), %ebx
      movl %ebp, (%ebx)

      movl 12(%ebp), %ebx 
      movl (%ebx), %esp 

      popl %ebp 
      ret 