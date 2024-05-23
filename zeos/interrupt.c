/*
 * interrupt.c -
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>
#include <libc.h>
#include <mm.h>

//importing INITIAL_ESP (which is the kernel initial ESP)
#include <sched.h>
#include <zeos_interrupt.h>

int switch_idle = 0;

int zeos_ticks = 0;

Gate idt[IDT_ENTRIES];
Register    idtR;
circular_buffer cbuff;

char char_map[] =
{
    '\0','\0','1','2','3','4','5','6',
    '7','8','9','0','\'','¡','\0','\0',
    'q','w','e','r','t','y','u','i',
    'o','p','`','+','\0','\0','a','s',
    'd','f','g','h','j','k','l','ñ',
    '\0','º','\0','ç','z','x','c','v',
    'b','n','m',',','.','-','\0','*',
    '\0','\0','\0','\0','\0','\0','\0','\0',
    '\0','\0','\0','\0','\0','\0','\0','7',
    '8','9','-','4','5','6','+','1',
    '2','3','0','\0','\0','\0','<','\0',
    '\0','\0','\0','\0','\0','\0','\0','\0',
    '\0','\0'
};

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
    /***********************************************************************/
    /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
    /* ***************************                                         */
    /* flags = x xx 0x110 000 ?????                                        */
    /*         |  |  |                                                     */
    /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
    /*         |   \ DPL = Num. higher PL from which it is accessible      */
    /*          \ P = Segment Present bit                                  */
    /***********************************************************************/
    Word flags = (Word)(maxAccessibleFromPL << 13);
    flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

    idt[vector].lowOffset       = lowWord((DWord)handler);
    idt[vector].segmentSelector = __KERNEL_CS;
    idt[vector].flags           = flags;
    idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
    /***********************************************************************/
    /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
    /* ********************                                                */
    /* flags = x xx 0x111 000 ?????                                        */
    /*         |  |  |                                                     */
    /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
    /*         |   \ DPL = Num. higher PL from which it is accessible      */
    /*          \ P = Segment Present bit                                  */
    /***********************************************************************/
    Word flags = (Word)(maxAccessibleFromPL << 13);

    //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
    /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
        the system calls will be thread-safe. */
    flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

    idt[vector].lowOffset       = lowWord((DWord)handler);
    idt[vector].segmentSelector = __KERNEL_CS;
    idt[vector].flags           = flags;
    idt[vector].highOffset      = highWord((DWord)handler);
}

void sys_call_handler_sysenter();

void set_msr(unsigned long msr_addr, unsigned long low, unsigned long high);

void set_sysenter_msrs(){
    set_msr(0x174, __KERNEL_CS, 0);
    set_msr(0x175, INITIAL_ESP, 0);
    set_msr(0x176, (unsigned long) sys_call_handler_sysenter, 0);
}

void setIdt()
{
    /* Program interrups/exception service routines */
    idtR.base  = (DWord)idt;
    idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;

    set_handlers();
    /* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */
    setInterruptHandler(32, clk_handler, 0);
    setInterruptHandler(33, kbd_handler, 0);
    //trap handler is only used for INT interruptions not SYSENTER
    //setTrapHandler(0x80, sys_call_handler, 3);

    set_sysenter_msrs();

    set_idt_reg(&idtR);
}

void initCircBuff()
{
    init(&cbuff);
}

void clk_routine(){
    ++zeos_ticks;
    zeos_show_clock();
    ++ticks_since_last_switch;

    schedule();
}

void kbd_routine(){
    Byte key = inb(0x60);
    Byte is_break = (key & 0x80) != 0x00;
    Byte scan_code = (key << 1) >> 1;  

    //Maybe scan_code should be checked
    if(!is_break){
        char character = char_map[scan_code];
        put(&cbuff, character);
        //printc_xy(0x00, 0x00, char_map[scan_code]);
    }
}

//si el num refs es 1 eres el unico que tiene esa pagina lo unico que tienes entonces solo cambias los bits de proteccion
void pgf_routine()
{	
	char buf[16];
	char * mesg = "\nProcess generates a PAGE FAULT exception at %eip = 0x";

	int pf_offender;
	asm("\t movl %%eax, %0" : "=r"(pf_offender));
    pf_offender = PH_PAGE(pf_offender);
    page_table_entry* pt = get_PT(current());
    
    if(is_cow_page(pt, (unsigned int) pf_offender)){
        //Magically copy a page
                
    }
    else{
        itohexa(pf_offender, buf);
        strcat(mesg, buf);
        
        printk(mesg);
        
        while (1) {};
    }

}

// SYSENTER MSRs:  CS (0x174),  ESP(0x175), @handler/EIP (0x176). SYSENTER pone los bits de privilege level PSW a 00
// SYSENTER es necesario crear el CtxHW
// MSR[ECX] <- EDX : EAX (concat)

