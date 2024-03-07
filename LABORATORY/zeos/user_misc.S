/*
 * user_misc.S - implementation of systemcall wrappers (libc.h) and other user related stuff
 */

#include <asm.h>

//MAYBE THE REGISTERS SHOULD BE CHECKED FOR THIS WRAPPERS 

//sysenter routine (saves ECX/EDX to avoid sysexit overwrite)

//PENDING !?
ENTRY(syscall_sysenter)
      push %ecx 
      push %edx 
      push $sysenter_return 

      //Fake dynamic link (sysenter does not link automatically)
      push %ebp
      mov %esp, %ebp 
      sysenter

//return sysenter routine 
ENTRY(sysenter_return)
      pop %ebp    # restore ebp
      pop %edx    # destroy @sysenter_return value 
      pop %edx    # restore edx 
      pop %ecx    # restore ecx
      ret 

//Write user wrapper
ENTRY(write)
      pushl %ebp;
      movl %esp, %ebp;

      //For some strange reason EBX/ESI/EDI must be saved
      pushl %ebx; // bytes 0 - 4
      pushl %esi; // bytes 4 - 8 
      pushl %edi; // bytes 8 - 12
      
      //Syscall code (write 4)
      movl $4, %eax; 

      //Parameters from write are stored into registers EBX/ECX/EDX
      movl 0x8(%ebp), %edx;   # fd 
      movl 0xc(%ebp), %ecx;   # buffer
      movl 0x10(%ebp), %ebx;  # size -> edx 

      //SAVE ECX/EDX AND EIP to return (SYSEXIT overwrites it)
      push %ecx 
      push %edx 
      push $sysenter_return  

      //Fake dynamic link (sysenter does not link automatically)
      push %ebp
      mov %esp, %ebp 
      sysenter

sysenter_return:
      pop %ebp    # restore ebp
      pop %edx    # destroy @sysenter_return value 
      pop %edx    # restore edx 
      pop %ecx    # restore ecx

      
      popl %edi; 
      popl %esi; 
      popl %ebx;

      //if eax is negative then jump to error (JUMP SIGN)
      test %eax, %eax; 
      js error;

      popl %ebp; 
      ret;



// General errno treatment by syscalls 
error:
      neg %eax;
      mov %eax, errno; 
      mov $-1, %eax;
      popl %ebp;
      ret;



ENTRY(gettime)
      push %ebp;
      movl %esp, %ebp;