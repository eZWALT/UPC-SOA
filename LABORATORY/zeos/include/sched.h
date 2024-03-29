/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <types.h>
#include <mm_address.h>

#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024

////////////////////// STRUCTURES

// PROCESS STATES
enum state_t { ST_RUN, ST_READY, ST_BLOCKED };

//PCB 
struct task_struct {
  int PID;			                         /* Process ID. This MUST be the first field of the struct. */
  page_table_entry * dir_pages_baseAddr; /* Direccio base de la Taula de Pagines (Unica)*/
  struct list_head fq_node;              /* Node de free Queue*/
  struct list_head rq_node;              /* Node de ready Queue*/
  unsigned long kernel_esp0;                      /* Direcció del cim del stack (Per restaurar)*/
};

//UNION (PCB + SYS STACK)
union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    
};

//TASK UNION VECTOR
extern union task_union task[NR_TASKS]; 
//POINTER TO IDLE PCB
extern struct task_struct* idle_task;

#define KERNEL_ESP(t)       	(DWord) &(t)->stack[KERNEL_STACK_SIZE]

#define INITIAL_ESP       	KERNEL_ESP(&task[1])




////////////////////////// FUNCTIONS


/* Inicialitza les dades del proces inicial */
void init_task1(void);
void init_idle(void);
void init_sched(void);

struct task_struct * current();

//Wrapper per guardar EBP, EBX, ESI, EDI
void task_switch(union task_union*t);

//Funcio principal
void inner_task_switch(union task_union*t);

//Part baix nivell de inner_task_switch
void switch_stacks(void ** sp, void * new_sp);

struct task_struct *list_head_to_task_struct(struct list_head *l);

int allocate_DIR(struct task_struct *t);

page_table_entry * get_PT (struct task_struct *t) ;

page_table_entry * get_DIR (struct task_struct *t) ;

/* Headers for the scheduling policy */
void sched_next_rr();
void update_process_state_rr(struct task_struct *t, struct list_head *dest);
int needs_sched_rr();
void update_sched_data_rr();

#endif  /* __SCHED_H__ */
