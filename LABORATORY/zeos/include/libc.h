/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>


int write(int fd, char *buffer, int size);

void itodeca(int a, char *b);

void itohexa(int a, char *b);

int strlen(char *a);

void strcat(char *dest, char* src);

int getpid();

int fork();

void exit();

extern int errno; 

void perror();

#endif  /* __LIBC_H__ */
