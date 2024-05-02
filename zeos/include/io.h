/*
 * io.h - Definició de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#include <types.h>
#include <circular.h>

extern Byte x_screen;
extern Byte y_screen;
extern Byte fg_color;
extern Byte bg_color;
/** Screen functions **/
/**********************/

Byte inb (unsigned short port);
void printc(char c);
void printc_xy(Byte x, Byte y, char c);
void printk(char *string);

#endif  /* __IO_H__ */
