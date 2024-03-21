/*
 * entry.S - Entry point to system mode from user mode
 */

#include <asm.h>
#include <segment.h>
#include <errno.h>

/**************************************************/
/**** Save & Restore ******************************/
/**                                              **/
/** When we change to privilege level 0 (kernel) **/
/** (through an interrupt, a system call, an     **/
/** exception ...) we must save the state of the **/
/** currently running task (save).               **/
/**                                              **/
/** Stack layout in 'systemCall':                **/
/**                                              **/
/**   0(%esp) - %edx    \                        **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %ebx     |                       **/
/**   C(%esp) - %esi     | Register saved        **/
/**  10(%esp) - %edi     |  by 'save'            **/
/**  14(%esp) - %ebp     |                       **/
/**  18(%esp) - %eax     |                       **/
/**  1C(%esp) - %ds      |                       **/
/**  20(%esp) - %es      |                       **/
/**  24(%esp) - %fs      |                       **/
/**  28(%esp) - %gs     /                        **/
/**  2C(%esp) - %eip    \                        **/
/**  30(%esp) - %cs      |                       **/
/**  34(%esp) - %eflags  |  Return context saved **/
/**  38(%esp) - %oldesp  |   by the processor.   **/
/**  3C(%esp) - %oldss  /                        **/
/**                                              **/
/**************************************************/

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
      movl $__KERNEL_DS, %edx; \    
      movl %edx, %ds; \           
      movl %edx, %es; \

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
      popl %gs; \

#define EOI \ 
movb $0x20, %al; \  
outb %al, $0x20; \

//NOTE: iret restores EFLAGS, CS, EIP back to normal values, ret only restores EIP (%ebp + 4)

ENTRY(kbd_handler) 
      SAVE_ALL 
      EOI      
      call kbd_routine;  
      RESTORE_ALL 
      iret; 

ENTRY(clk_handler)
      SAVE_ALL 
      EOI 
      call clk_routine;
      RESTORE_ALL
      iret;

ENTRY(__wrap_page_fault_handler)
	  movl 0x30(%esp), %eax;   /* Saving to %eax the %eip that caused the exception */
	  call pgf_routine;

#THIS HANDLER SHOULD NOT BE USED AT ALL (Too sus)
ENTRY(sus_call_handler)
      SAVE_ALL
      cmpl $0, %eax
      jl err
      cmpl $MAX_SYSCALL, %eax
      jg err
      call *sys_call_table(, %eax, 0x04)
      jmp fin
err:
      movl $-ENOSYS, %eax 
fin:
      movl %eax, 0x18(%esp)
      RESTORE_ALL 
      iret

ENTRY(sys_call_handler_sysenter)
      //PUSHING CtxHW
      push $__USER_DS         #push user data segment         
      push %ebp               #push user stack address 
      pushfl                  #push EFLAGS (Processor Status Word)
      push $__USER_CS         #push user code segment 
      push 0x4(%ebp)          #push return address 

      //PUSHING CtxSW
      SAVE_ALL

      //ERROR CHECKING
      cmpl $0, %eax           # eax < 0 
      jl sysenter_err         

      cmpl $MAX_SYSCALL, %eax # eax < MAX_SYSCALL
      jg sysenter_err

      //PEGA UN PUTO CALL A SYS_XXX
      call *sys_call_table(, %eax, 0x04)
      jmp sysenter_end 
sysenter_err: 
      movl $-ENOSYS, %eax     #eax = - ENOSYS
sysenter_end: 
      movl %eax, 0x18(%esp)   #overwrite eax to avoid losing error code during RESTORE_ALL 
      RESTORE_ALL
      movl (%esp), %edx       # return address
      movl 0xc(%esp), %ecx    # user stack adress 
      sti                     # enable interrupts 
      sysexit                 # sysexit goes back to user level privilege and restores EIP , ESP
