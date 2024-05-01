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
#include <types.h>

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

// Declaration of ret_from_fork
int ret_from_fork();

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

    struct task_struct* child_pcb = list_head_to_task_struct(elem);
    union task_union* child_union = (union task_union*) child_pcb; 
    union task_union* parent_union = (union task_union*) current();
    // Copying entire union task_union
    copy_data(parent_union->stack, child_union->stack, PAGE_SIZE);

    // allocate a new directory table
    allocate_DIR(child_pcb);

    // search for frames to allocate
    page_table_entry* pt_child  = get_PT(child_pcb);
    page_table_entry* pt_parent = get_PT(current());

    int new_ph_pag, old_ph_pag, pag;

    /* KERNEL */
    for (pag=0; pag < NUM_PAG_KERNEL; pag++) 
        pt_child[pag] = pt_parent[pag];

    /* CODE */
    for (pag=0; pag < NUM_PAG_CODE; pag++)
        pt_child[PAG_LOG_INIT_CODE+pag] = pt_parent[PAG_LOG_INIT_CODE+pag];

    /* DATA */

    // KERNEL (256) + (NUM_PAG_DATA + NUM_PAG_CODE) | --> FREE
    for (pag=0; pag < NUM_PAG_DATA; pag++){
        new_ph_pag = alloc_frame();

        if (new_ph_pag < 0) {
            free_user_pages(child_pcb);
            return -ENOMEM;
        }

        set_ss_pag(pt_parent, USER_LAST_PAGE + pag, new_ph_pag);
        set_ss_pag(pt_child, USER_FIRST_PAGE + pag, new_ph_pag);

        copy_data((void *)((USER_FIRST_PAGE + pag) << 12), (void *)((USER_LAST_PAGE + pag) << 12), PAGE_SIZE);

        del_ss_pag(pt_parent, USER_LAST_PAGE + pag);
    }

    // Flush of the Translation Lookaside Buffer
    set_cr3(get_DIR(current()));

    // Process ID and NR_TICKS and the state
    child_pcb->PID = next_pid++;
    child_pcb->nr_ticks = 0;
    child_pcb->state = ST_READY;
    child_pcb->pending_unblocks = 0;
    // Insert the child to the RQ
    list_add_tail( &(child_pcb->node), &readyqueue);

    // Prepare child's system stack
    child_union->stack[KERNEL_STACK_SIZE - 18] = (unsigned long) &ret_from_fork;
    child_union->stack[KERNEL_STACK_SIZE - 19] = 0;

    // Update kernel_esp
    child_pcb->kernel_esp0 = (unsigned long *) &child_union->stack[KERNEL_STACK_SIZE - 19];

    // Pointers in familiy tree
    child_pcb->parent = current();
    INIT_LIST_HEAD(&child_pcb->sons);
    list_add_tail( &child_pcb->bros, &current()->sons);

    // return
    return child_pcb->PID;
}

void sys_exit()
{  
    //Mark the process PID as free
    struct task_struct * proc_pcb = current();
    union task_union * proc_union = (union task_union*) proc_pcb;

    if (proc_pcb->PID != 1) list_del(&proc_pcb->bros);

    proc_pcb->PID = -1;

    //char buff[64];
    //itodeca(list_size(&proc_pcb->sons), buff);
    //printk(buff);

    struct list_head * e, *tmp;

    for (int i = 0; i < NR_TASKS; ++i)
    {   
        if (task[i].task.parent != current()) continue;

        task[i].task.parent = idle_task;
        list_del(&task[i].task.bros);
        list_add_tail(&task[i].task.bros, &idle_task->sons);
    }


    //Schedule the next process to be executed
    update_process_state_rr(proc_pcb, &freequeue, ST_ZOMBIE);

    //Free the allocated phyiscal frames and page directory
    free_user_pages(proc_pcb);
    proc_pcb->dir_pages_baseAddr = NULL;

    sched_next_rr();
}

//System gettime
int sys_gettime(){
    return zeos_ticks;
}

int sys_block(){
    struct task_struct* proc_pcb = current();
    //Node is added to blocked list
    if(proc_pcb->pending_unblocks == 0){

        //Put it into the blocked queue, since its running it shouldn't be in any queue
        list_add_tail(&(proc_pcb->node), &blocked);
        proc_pcb->state = ST_BLOCKED;
        //Schedule a new process now that it's blocked
        schedule();
    }
    else proc_pcb->pending_unblocks--; 
    
    return 0;
}

int sys_unblock(int pid){
    struct task_struct* proc_pcb = current();
    struct list_head *pos,*n;

    //find the child with PID == pid
    list_for_each_safe(pos, n, &proc_pcb->sons){
        struct task_struct* son = list_head_to_task_struct(pos);
        if(son->PID == pid){
            //if son blocked, unblock it!
            if(son->state == ST_BLOCKED){
                son->state = ST_READY;
                son->pending_unblocks = 0;
                list_del(&(son->node));
                list_add_tail(&(son->node), &readyqueue);
            }
            else son->pending_unblocks++;
            //if son ublocked, just ++
            return 0;
        }
    }
    //If no son is found...
    return -ECHILD;
}

//Additional process syscalls

int sys_numsons(){
    return list_size(&(current()->sons)) - 1;
}

int sys_numbros(){
    return list_size(&(current()->bros)) - 1;
}

//Videogame syscalls
int sys_read(char* b, int maxchars){
    int num_chars = 0;
    for(int i = 0; i < maxchars; ++i){
        
    }
}

int sys_gotoxy(int x, int y){
    return -ENOSYS;
}

int sys_set_color(int fg, int bg){
    return -ENOSYS;
}

//Shared memory syscalls
void* sys_shmat(int id, void* addr){
    return -ENOSYS;
}

void sys_shmdt(void* addr){
    return -ENOSYS;
}

int sys_shmrm(int id){
    return -ENOSYS;
}