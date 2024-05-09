/*
 * io.c -
 */

#include <colors.h>
#include <io.h>
#include <types.h>

/**************/
/** Screen  ***/
/**************/

Byte x_screen, y_screen = 19;
Byte fg_color = MAGENTA, bg_color = BLACK;

/* Read a byte from 'port' */
Byte inb(unsigned short port) {
    Byte v;

    __asm__ __volatile__("inb %w1,%0" : "=a"(v) : "Nd"(port));
    return v;
}

void printc(char c) {
    __asm__ __volatile__("movb %0, %%al; outb $0xe9" ::"a"(
        c)); /* Magic BOCHS debug: writes 'c' to port 0xe9 */
    if (c == '\n') {
        x_screen = 0;
        y_screen = (y_screen + 1) % SCREEN_ROWS;
    } else {
        Word ch = (Word)(c & 0x00FF) | (bg_color << 12) | (fg_color << 8);
        Word *screen = (Word *)0xb8000;
        screen[(y_screen * SCREEN_COLUMNS + x_screen)] = ch;
        if (++x_screen >= SCREEN_COLUMNS) {
            x_screen = 0;
            y_screen = (y_screen + 1) % SCREEN_ROWS;
        }
    }
}

void printc_xy(Byte mx, Byte my, char c) {
    Byte cx, cy;
    cx = x_screen;
    cy = y_screen;
    x_screen = mx;
    y_screen = my;
    printc(c);
    x_screen = cx;
    y_screen = cy;
}

void printk(char *string) {
    int i;
    for (i = 0; string[i]; i++)
        printc(string[i]);
}
