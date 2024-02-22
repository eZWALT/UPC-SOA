# 0 "entry.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "entry.S"




# 1 "include/asm.h" 1
# 6 "entry.S" 2
# 1 "include/segment.h" 1
# 7 "entry.S" 2
# 1 "include/interrupt.h" 1







# 1 "include/types.h" 1
# 11 "include/types.h"
typedef unsigned char Byte;
typedef unsigned short int Word;
typedef unsigned long DWord;







typedef struct
{
  Word limit;
  Word lowBase;
  Byte midBase;
  Byte flags1;
  Byte flags2;
  Byte highBase;
} Descriptor;

typedef struct
{
  Word lowOffset;
  Word segmentSelector;
  Word flags;
  Word highOffset;
} Gate;

typedef struct
{
  Word PreviousTaskLink;
  Word none1;
  DWord esp0;
  Word ss0;
  Word none2;
  DWord esp1;
  Word ss1;
  Word none3;
  DWord esp2;
  Word ss2;
  Word none4;
  DWord cr3;
  DWord eip;
  DWord eFlags;
  DWord eax;
  DWord ecx;
  DWord edx;
  DWord ebx;
  DWord esp;
  DWord ebp;
  DWord esi;
  DWord edi;
  Word es;
  Word none5;
  Word cs;
  Word none6;
  Word ss;
  Word none7;
  Word ds;
  Word none8;
  Word fs;
  Word none9;
  Word gs;
  Word none10;
  Word LDTSegmentSelector;
  Word none11;
  Word debugTrap;
  Word IOMapBaseAddress;
} TSS;




typedef struct
{
  Word limit __attribute__ ((packed));
  DWord base __attribute__ ((packed));
} Register;
# 100 "include/types.h"
typedef Word Selector;
# 136 "include/types.h"
typedef union
{
  unsigned int entry;
  struct {
    unsigned int present : 1;
    unsigned int rw : 1;
    unsigned int user : 1;
    unsigned int write_t : 1;
    unsigned int cache_d : 1;
    unsigned int accessed : 1;
    unsigned int dirty : 1;
    unsigned int ps_pat : 1;
    unsigned int global : 1;
    unsigned int avail : 3;
    unsigned int pbase_addr : 20;
  } bits;
} page_table_entry;
# 9 "include/interrupt.h" 2



extern Gate idt[256];
extern Register idtR;

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL);
void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL);

void setIdt();
# 8 "entry.S" 2
# 73 "entry.S"
.globl kbd_handler; .type kbd_handler, @function; .align 0; kbd_handler: pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %ebx; pushl %ecx; pushl %edx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es; movb $0x20, %al; outb %al, $0x20; call kbd_routine; popl %edx; popl %ecx; popl %ebx; popl %esi; popl %ebp; popl %eax; popl %ds; popl %es; popl %fs; popl %gs; iret;
