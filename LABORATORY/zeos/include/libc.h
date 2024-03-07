/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

#include <stats.h>


//string utils
void itodeca(int a, char *b);
void itohexa(int a, char *b);
int strlen(char *a);
void strcat(char *dest, char* src);
void strcpy(char *dest, char* src);

//syscalls wrappers
int write(int fd, char *buffer, int size);
int gettime();
int getpid();
int fork();
void exit();

extern int errno; 

void perror();

#endif  /* __LIBC_H__ */
