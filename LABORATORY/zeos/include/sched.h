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
enum state_t { ST_RUN, ST_READY, ST_BLOCKED, ST_ZOMBIE};

//PCB 
struct task_struct {
    int PID;			                   /* Process ID. This MUST be the first field of the struct. */
    page_table_entry * dir_pages_baseAddr; /* Base address of page directory */
    struct list_head node;                 /* Node for state queues (FQ, RQ, BQ) */
    enum state_t state;                    /* State of the process */

    struct list_head dead_sons;            /* Dead Children list (Zombie!)*/
    struct list_head sons;                 /* Alive Children list */
    struct list_head bros;                 /* Brothers list */
    struct task_struct* parent;            /* Pointer to father's PCB */

    unsigned long kernel_esp0;             /* Reference to top of system stack */
    unsigned int nr_ticks;                 /* Number of ticks process has spent using CPU */
    unsigned int quantum;                  /* CPU Quantum */
    int pending_unblocks;                  /* Number of pending unblocks left */ 
};

//This function is actually wrong, it should use exit on each children
void kill_all_my_fucking_children(struct task_struct* process);

//UNION (PCB + SYS STACK)
union task_union {
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    
};

//TASK UNION VECTOR
extern union task_union task[NR_TASKS]; 
//POINTER TO IDLE PCB
extern struct task_struct* idle_task;
//POINTER TO TASK1 PCB
extern struct task_struct* task1_task;
// NEXT PID
extern int next_pid;
// RQ and FQ
extern struct list_head freequeue, readyqueue, blocked;
// Number of tiks since last task switch
extern unsigned int ticks_since_last_switch;

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
void switch_stacks(unsigned long * sp, unsigned long new_sp);

struct task_struct *list_head_to_task_struct(struct list_head *l);

int allocate_DIR(struct task_struct *t);

page_table_entry * get_PT (struct task_struct *t) ;

page_table_entry * get_DIR (struct task_struct *t) ;

/* Headers for the scheduling policy */
void schedule();
void sched_next_rr();
//  dest_state should be updated according to the state that the process will enter
void update_process_state_rr(struct task_struct *t, struct list_head *dest, enum state_t dest_state);
int needs_sched_rr();
void update_sched_data_rr();
int get_quantum(struct task_struct* t);
void set_quantum(struct task_struct* t, int new_quantum);

#endif  /* __SCHED_H__ */
