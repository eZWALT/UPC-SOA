/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <types.h>
#include <sched.h>
#include <mm.h>
#include <io.h>

/*
//This function is actually wrong, it should use exit on each children
void kill_all_my_fucking_children(struct task_struct* process){
  struct list_head *son;
  list_for_each(son, &process->sons){
    list_del(son);
    list_add_tail(son, &process->dead_sons);
  }
}
*/

//INITIALIZE THE DATA STRUCTURES DESCRIBED IN SCHED.H
union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

struct task_struct * idle_task  = NULL;
struct task_struct * task1_task = NULL;
int next_pid = 2;
unsigned int ticks_since_last_switch;

//THIS FUNCTION SHOULD NEVER BE USED FROM LIST HEADS 
struct task_struct * list_head_to_task_struct(struct list_head *l)
{
	return (struct task_struct *)((unsigned long) l & 0xFFFFF000);
}

struct list_head freequeue;
struct list_head readyqueue;
// Queue for blocked processes in I/O 
struct list_head blocked;

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{	
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned long)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

    printk("\nEntering IDLE\n");

	while(1)
	{
	}
}

void init_idle (void)
{
	// Get a free PCB and remove it from FQ
	struct list_head * elem = list_first(&freequeue);
	list_del(elem);

	struct task_struct* idle_pcb = list_head_to_task_struct(elem);
	union task_union* idle_union = (union task_union*) idle_pcb; 
	
	// Assign a PID of 0 and allocate a new directory table
	idle_pcb->PID = 0;
	idle_pcb->nr_ticks = 0;
	allocate_DIR(idle_pcb);

    // Family
    INIT_LIST_HEAD(&idle_pcb->sons);

	// Load dynamic link with return value and fake ebp
	idle_union->stack[KERNEL_STACK_SIZE-2] = 0;
	idle_union->stack[KERNEL_STACK_SIZE-1] = (unsigned long) &cpu_idle;

	// kernel_esp for Idle's task struct 
	idle_pcb->kernel_esp0 = (unsigned long) &(idle_union->stack[KERNEL_STACK_SIZE-2]);
	// Save reference to PCB for later when needed
	idle_task = idle_pcb;

}

void set_msr(unsigned long msr_addr, unsigned long low, unsigned long high);

void init_task1(void)
{
	// Get a free PCB and remove it from FQ
	struct list_head * elem = list_first(&freequeue);
	list_del(elem);

	struct task_struct* task1_pcb = list_head_to_task_struct(elem);
	union task_union* task1_union = (union task_union*) task1_pcb; 

	// Assign a PID of 1 and allocate a new directory table
	task1_pcb->PID = 1;
    task1_pcb->nr_ticks = 0;
	//This process will always be the first to RUN without task_switch
	task1_pcb->state = ST_RUN;

	//Initialize the lists of brothers  and sons 
	INIT_LIST_HEAD(&task1_pcb->sons);

	// Allocate pages to task1
	allocate_DIR(task1_pcb);
	set_user_pages(task1_pcb);

	// Update TSS and MSR with the base address of task1_union 
	tss.esp0 = (unsigned long) &(task1_union->stack[KERNEL_STACK_SIZE]);
	set_msr(0x175, (unsigned long)&(task1_union->stack[KERNEL_STACK_SIZE]), 0);

	// Update %cr3 and flush TLB
	set_cr3(task1_pcb->dir_pages_baseAddr);

	// Save reference to PCB for later when needed
	task1_task = task1_pcb;

    // Update ticks since last task switch
    ticks_since_last_switch = 0;

    // Quantum
    task1_pcb->quantum = 200;
}

void inner_task_switch(union task_union* new)
{
	// Get the directory of new task
	page_table_entry * new_dir = get_DIR(&(new->task));

	// Update TSS and MSR with the base address of new
	tss.esp0 = (unsigned long) &(new->stack[KERNEL_STACK_SIZE]);
	set_msr(0x175, (unsigned long) &(new->stack[KERNEL_STACK_SIZE]), 0);

	// Update %cr3 and flush TLB 
	set_cr3(new_dir);


	// current()->kernel_esp0 = %ebp
	// %esp = new->task.kernel_esp0
	switch_stacks(&(current()->kernel_esp0), new->task.kernel_esp0);
}

void schedule()
{
    // Updates ticks
    update_sched_data_rr();

    if (needs_sched_rr())
        sched_next_rr();
}

// Initializes FQ, RQ and BQ
void init_sched()
{
	INIT_LIST_HEAD(&freequeue);
	for(unsigned long i = 0; i < NR_TASKS; i++){
		// All tasks in FQ are invalid
		task[i].task.PID = -1;
		list_add(&(task[i].task.node), &freequeue);
	}

	INIT_LIST_HEAD(&readyqueue);
	INIT_LIST_HEAD(&blocked);
}

struct task_struct* current()
{
    int ret_value;

    __asm__ __volatile__(
    "movl %%esp, %0"
    : "=g" (ret_value)
    );
    return (struct task_struct*)(ret_value&0xfffff000);
}

void update_sched_data_rr()
{
    struct task_struct* proc_pcb = current();
    proc_pcb->nr_ticks = ticks_since_last_switch;
}

// Returns 1 if we need a task switch, 0 otherwise
int needs_sched_rr()
{
    struct task_struct* proc_pcb = current();
    
    // Are we idle
    if (proc_pcb->PID == 0) return !list_empty(&readyqueue);
    // Has our quantum expired?
    if (get_quantum(proc_pcb) < proc_pcb->nr_ticks)
    {
        // Is RQ empty?
        return !list_empty(&readyqueue);
    }
	//is the current process blocked??
	if(proc_pcb->state == ST_BLOCKED) return !list_empty(&readyqueue);

    return 0;
}

void update_process_state_rr(struct task_struct *t, struct list_head *dest, enum state_t dest_state){    

    if (dest == NULL){
		list_del(&t->node);
		t->state = ST_RUN;
	} 
    else if (current() == t){
		list_add_tail(&t->node, dest);
		t->state = dest_state;
	}
    else {
        list_del(&t->node);
        list_add_tail(&t->node, dest);
		t->state = dest_state;
    }
    
}

void sched_next_rr()
{
    if (list_empty(&readyqueue))
    {
        // IDLE time !
        ticks_since_last_switch = 0;
        task_switch(&idle_task);
		return;
    }


    struct list_head* new_list_item = list_first(&readyqueue);
    struct task_struct* new = list_head_to_task_struct(new_list_item);

    if (current()->PID != 0) update_process_state_rr(current(), &readyqueue, ST_READY);
    update_process_state_rr(new, NULL, ST_RUN);
    // Update ticks since last task switch
    ticks_since_last_switch = 0;

    task_switch(new);
	return;
}

int get_quantum(struct task_struct* t)
{
    return t->quantum;
}

void set_quantum(struct task_struct* t, int new_quantum)
{
    t->quantum = new_quantum;
}