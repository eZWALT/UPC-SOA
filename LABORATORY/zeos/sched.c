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

struct task_struct * idle_task = NULL;


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
	;
	}
}

void init_idle (void)
{
	//Obté el primer element
	struct list_head * elem = list_first(&freequeue);
	//Esborra el list node de la llista, segueix en el PCB d'aquell proces (En aquest moment cap)
	list_del(elem);
	struct task_struct* idle_pcb = list_head_to_task_struct(elem);
	union task_union* idle_union = (union task_union*) idle_pcb; 
	
	//Asigna PID 0 i també asigna un directori de pagines
	idle_pcb->PID = 0;
	int success = allocate_DIR(idle_pcb);

	//Carrega el enllaç dinàmic amb la direccio de retorn i el valor del ebp (0)
	idle_union->stack[KERNEL_STACK_SIZE-1] = (unsigned long) &cpu_idle;
	idle_union->stack[KERNEL_STACK_SIZE-2] = 0;

	//Registrem el valor de %esp 
	idle_pcb->kernel_esp0 = (unsigned long) &(idle_union->stack[KERNEL_STACK_SIZE-2]);

	//Registrem aquesta tasca en un ambit global (PERO NO LA ENCUEM MAI!!!)
	idle_task = idle_pcb;

}

void set_msr(unsigned long msr_addr, unsigned long low, unsigned long high);

void get_ebp();

void inner_task_switch_asm(union task_union* new);

void init_task1(void)
{
	//Obte un element
	struct list_head * elem = list_first(&freequeue);
	//Esborrem aquesta entrada
	list_del(elem);
	struct task_struct * init_pcb = list_head_to_task_struct(elem);
	union task_union * init_union = (union task_union*) init_pcb;

	//Assignem PID i Entrada del Directori de págines
	init_pcb->PID = 1;
	allocate_DIR(init_pcb);

	//Assignem unes cuantes págines al usuari d'aquest proces
	set_user_pages(init_pcb);

	//Actualitzem el valor del stack TSS 
	tss.esp0 = (unsigned long) &(init_union->stack[KERNEL_STACK_SIZE]);
	//PREGUNTA PER EL PROFE: PERQUE NO CAL POSAR SS0? tss.ss0 = ...;

	//Actualitzem MSR 0x175 amb la direccio nova del stack
	set_msr(0x175, tss.esp0, 0);

	//CR3 <- TP del nou proces
	set_cr3(init_pcb->dir_pages_baseAddr);
}

void inner_task_switch(union task_union* new){
	//obte NEW directori de paginaes
	page_table_entry * NEW_DIR = get_DIR(&new->task);
	//Tant el esp0 de TSS com MSR 0x175 apuntant a la base de la pila de NEW 
	tss.esp0 = 	&(new->stack[KERNEL_STACK_SIZE]);
	set_msr(0x175, &(new->stack[KERNEL_STACK_SIZE]),0);
	//Posa NEW directori a CR3 i TLB flush 
	set_cr3(NEW_DIR);

	// current()->kernel_esp0 = %ebp 
	// %esp <- new->task.kernek_esp0
	switch_stacks(&current()->kernel_esp0, new->task.kernel_esp0);
	

}

//Incialitza la freequeue y la readyqueu
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

