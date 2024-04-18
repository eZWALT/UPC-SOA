# 0 "entry.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "entry.S"




# 1 "include/asm.h" 1
# 6 "entry.S" 2
# 1 "include/segment.h" 1
# 7 "entry.S" 2
# 1 "include/errno.h" 1
# 8 "entry.S" 2
# 73 "entry.S"
.globl kbd_handler; .type kbd_handler, @function; .align 0; kbd_handler:
      pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %ebx; pushl %ecx; pushl %edx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es;
      movb $0x20, %al; outb %al, $0x20;
      call kbd_routine;
      popl %edx; popl %ecx; popl %ebx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
      iret;

.globl clk_handler; .type clk_handler, @function; .align 0; clk_handler:
      pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %ebx; pushl %ecx; pushl %edx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es;
      movb $0x20, %al; outb %al, $0x20;
      call clk_routine;
      popl %edx; popl %ecx; popl %ebx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
      iret;

.globl __wrap_page_fault_handler; .type __wrap_page_fault_handler, @function; .align 0; __wrap_page_fault_handler:
   movl 0x04(%esp), %eax;
   call pgf_routine;

#THIS HANDLER SHOULD NOT BE USED AT ALL (Too sus)
.globl sus_call_handler; .type sus_call_handler, @function; .align 0; sus_call_handler:
      pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %ebx; pushl %ecx; pushl %edx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es;
      cmpl $0, %eax
      jl err
      cmpl $MAX_SYSCALL, %eax
      jg err
      call *sys_call_table(, %eax, 0x04)
      jmp fin
err:
      movl $-40, %eax
fin:
      movl %eax, 0x18(%esp)
      popl %edx; popl %ecx; popl %ebx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
      iret

.globl sys_call_handler_sysenter; .type sys_call_handler_sysenter, @function; .align 0; sys_call_handler_sysenter:

      push $0x2B #push user data segment
      push %ebp #push user stack address
      pushfl #push EFLAGS (Processor Status Word)
      push $0x23 #push user code segment
      push 0x4(%ebp) #push return address


      pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %ebx; pushl %ecx; pushl %edx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es;


      cmpl $0, %eax # eax < 0
      jl sysenter_err

      cmpl $MAX_SYSCALL, %eax # eax < MAX_SYSCALL
      jg sysenter_err


      call *sys_call_table(, %eax, 0x04)
      jmp sysenter_end
sysenter_err:
      movl $-40, %eax #eax = - 40
sysenter_end:
      movl %eax, 0x18(%esp) #overwrite eax to avoid losing error code during popl %edx; popl %ecx; popl %ebx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
      popl %edx; popl %ecx; popl %ebx; popl %esi; popl %edi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs;
      movl (%esp), %edx # return address
      movl 0xc(%esp), %ecx # user stack adress
      sti # enable interrupts
      sysexit # sysexit goes back to user level privilege and restores EIP , ESP
