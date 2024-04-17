#include <asm.h>
#include <segment.h>

ENTRY(sys_call_table)
	.long sys_ni_syscall		/* 0 */
	.long sys_exit 		    	/* 1 */
	.long sys_fork				/* 2 */ 
	.long sys_ni_syscall 
	.long sys_write				/* 4 */
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall		/* 8 */
	.long sys_ni_syscall
	.long sys_gettime   		/* 10 */
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_block				/* 18 */
	.long sys_unblock			/* 19 */
	.long sys_getpid			/* 20 */
	.long sys_numsons			/* 21 */
	.long sys_numbros			/* 22 */

/* Syscalls starting from 20 to ... are getters for process data */

.globl MAX_SYSCALL
MAX_SYSCALL = (. - sys_call_table)/4