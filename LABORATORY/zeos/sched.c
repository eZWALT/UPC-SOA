/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <types.h>
#include <sched.h>
#include <mm.h>
#include <io.h>

//INITIALIZE THE DATA STRUCTURES DESCRIBED IN SCHED.H
union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

struct task_struct * idle_task  = NULL;
struct task_struct * task1_task = NULL;
int next_pid = 2;

//THIS FUNCTION SHOULD NEVER BE USED FROM LIST HEADS 
struct task_struct * list_head_to_task_struct(struct list_head *l)
{
	return (struct task_struct *)((unsigned long) l & 0xFFFFF000);
}

extern struct list_head blocked;
struct list_head freequeue;
struct list_head readyqueue;

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
	allocate_DIR(idle_pcb);
    //idle_pcb->nr_ticks = 0;

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
    //task1_pcb->nr_ticks = 0;
	allocate_DIR(task1_pcb);

	// Allocate pages to task1
	set_user_pages(task1_pcb);

	// Update TSS and MSR with the base address of task1_union 
	tss.esp0 = (unsigned long) &(task1_union->stack[KERNEL_STACK_SIZE]);
	set_msr(0x175, (unsigned long)&(task1_union->stack[KERNEL_STACK_SIZE]), 0);

	// Update %cr3 and flush TLB
	set_cr3(task1_pcb->dir_pages_baseAddr);

	// Save reference to PCB for later when needed
	task1_task = task1_pcb;
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

// Initializes FQ and RQ
void init_sched()
{
	INIT_LIST_HEAD(&freequeue);
	for(unsigned long i = 0; i < NR_TASKS; i++){
		//Totes les tasques de la freeQ son invalides (lliures)
		task[i].task.PID=-1;
		list_add(&(task[i].task.fq_node), &freequeue);
	}

	INIT_LIST_HEAD(&readyqueue);
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

