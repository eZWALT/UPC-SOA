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

      pushl 8(%ebp)
      call inner_task_switch
      addl 4, %esp 

      popl %ebx 
      popl %edi 
      popl %esi 

      popl %ebp 
      ret 

//void switch_stacks(void ** sp + 4, void * new_sp + 8)
ENTRY(switch_stacks)
      //eax <- sp
      movl 4(%esp), %eax 
      //Emagatzema %ebp en el cim del sp (Quan torni el proces la seva pila seguira desde aqui)
      movl %ebp, (%eax) 
      //mou el valor del new_sp dins de %esp
      movl 8(%esp), %esp 
      // restaura %ebp i %eip  (Tornem a task_switch)
      popl %ebp 
      ret 
