/*
 * mm.h - Capçalera del mòdul de gestió de memòria
 */

#ifndef __MM_H__
#define __MM_H__

#include <types.h>
#include <mm_address.h>
#include <sched.h>

 
#define FREE_FRAME 0
#define NUM_SHARED_PAGES 10

/* Bytemap to mark the free physical pages */
// Each entry is the number of references to the frame.
extern Byte phys_mem[TOTAL_PAGES];

extern SharedPage shared_pages[NUM_SHARED_PAGES];
extern page_table_entry dir_pages[NR_TASKS][TOTAL_PAGES];

int init_frames( void );
int alloc_frame( void );
void free_frame( unsigned int frame );
void set_user_pages( struct task_struct *task );
void free_user_pages( struct task_struct *task );
int get_user_free_page( struct task_struct *task );
int is_frame_shared( unsigned int frame );
int is_logic_page_free(page_table_entry* PT, unsigned page);

extern Descriptor  *gdt;

extern TSS         tss; 

void init_mm();
void init_shared_pages();
void set_cr3(page_table_entry *dir);

void setGdt();

void setTSS();

void set_ss_pag(page_table_entry *PT, unsigned page,unsigned frame);
void del_ss_pag(page_table_entry *PT, unsigned page);
unsigned int get_frame(page_table_entry *PT, unsigned int page);
int is_cow_page(page_table_entry* pte, unsigned page);

#endif  /* __MM_H__ */
