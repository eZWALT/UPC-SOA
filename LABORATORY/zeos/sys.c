/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>
#include <utils.h>
#include <io.h>
#include <mm.h>
#include <mm_address.h>
#include <sched.h>
#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1
#define MAX_BUFFER_SIZE 512

char kern_buff[MAX_BUFFER_SIZE];

extern int zeos_ticks;

int check_fd(int fd, int permissions)
{
    if (fd!=1) return -9; /*EBADF*/
    if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
    return 0;
}

/**
 * Creates a new process by duplicating the calling process.
 *
 * This function creates a new process, the child process, which is an exact copy
 * of the calling process, the parent process. Both processes start execution
 * at the same point in the code, but they have separate memory spaces.
 *
 * @return On success, the process ID of the child process is returned in the parent
 * process, and 0 is returned in the child process. On failure, -1 is returned,
 * and errno is set to indicate the error.
 */
int sys_fork()
{
    // Allocate new free PCB
    if (list_empty(&freequeue)) 
        return -ENOMEM; // not enough resources

    struct list_head* elem = list_first(&freequeue);
    list_del(elem);

    struct taks_struct* child_pcb = list_head_to_task_struct(elem);
    union task_union* child_union = (union task_union*) child_pcb; 

    // Copying entire union task_union
    copy_data(current()->stack, child_union->stack, PAGE_SIZE);

    // allocate a new directory table
    allocate_DIR(child_pcb);

    // search for frames to allocate
    struct page_table_entry* pt_child  = get_PT(child_pcb);
    struct page_table_entry* pt_parent = get_PT(current()->task);

    int pag; 
    int new_ph_pag;

    /* CODE */
    for (pag=0;pag<NUM_PAG_CODE;pag++){

        pt_child[PAG_LOG_INIT_CODE+pag] = pt_parent[PAG_LOG_INIT_CODE+pag];
    }

    /* DATA */ 
    for (pag=0;pag<NUM_PAG_DATA;pag++){
        new_ph_pag=alloc_frame();
        if (new_ph_pag < 0) return -ENOMEM; // Not enough free frames

        process_PT[PAG_LOG_INIT_DATA+pag].entry = 0;
        process_PT[PAG_LOG_INIT_DATA+pag].bits.pbase_addr = new_ph_pag;
        process_PT[PAG_LOG_INIT_DATA+pag].bits.user = 1;
        process_PT[PAG_LOG_INIT_DATA+pag].bits.rw = 1;
        process_PT[PAG_LOG_INIT_DATA+pag].bits.present = 1;
    }

    return PID;
}


/**
 * Writes data to a file descriptor.
 *
 * This function writes data from the specified buffer to the file associated with the file descriptor.
 * The amount of data written may be less than requested if there is insufficient space available.
 *
 * @param fd The file descriptor to write to.
 * @param buffer Pointer to the buffer containing the data to be written.
 * @param size The number of bytes to write.
 * @return Upon successful completion, the number of bytes written is returned. On error, -1 is returned,
 * and errno is set appropriately to indicate the error.
 */
int sys_write(int fd, char * buffer, int size)
{
    int ret = check_fd(fd, ESCRIPTURA);
    int remaining_bytes = size;

    // Check the file descriptor
    if (ret) return ret;

    // Check if buffer is not null
    if (!access_ok(VERIFY_READ, buffer, size)) return -EFAULT; /* EFAULT */

    // Check size is positive
    if (size < 0) return -EINVAL; /* EINVAL */

    // Writing through console is done by chunks of 512 bytes each at a time 
    while(remaining_bytes > MAX_BUFFER_SIZE){
        // Copy 512 bytes (USER -> KERNEL)
        copy_from_user(buffer, kern_buff, MAX_BUFFER_SIZE);
        // Write 512 bytes to console
        ret = sys_write_console(kern_buff, MAX_BUFFER_SIZE);
        remaining_bytes -= ret;
        buffer += ret;
    }   

    if(remaining_bytes > 0){
        copy_from_user(buffer,kern_buff, remaining_bytes);
        ret = sys_write_console(kern_buff, remaining_bytes);
        remaining_bytes -= ret;
    }  

    return size - remaining_bytes;
}

int sys_ni_syscall()
{
    return -38; /*ENOSYS*/
}

/*
  Returns the Process ID of the current process that is being executed
*/
int sys_getpid()
{
	return current()->PID;
}


void sys_exit()
{  
}

//System gettime
int sys_gettime(){
    return zeos_ticks;
}

