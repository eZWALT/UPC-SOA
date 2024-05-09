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

//Game syscalls wrappers
int read(char* b, int maxchars);
int gotoxy(int x, int y);
int set_color(int fg, int bg);

//Shared memory wrappers
void* shmat(int id, void* addr);
int shmdt(void* addr);
int shmrm(int id);

//Custom syscalls for current process
int numsons();
int numbros();

//syscalls wrappers
int write(int fd, char *buffer, int size);
int gettime();
int getpid();
int fork();
void exit();

void block();
int unblock(int pid);

extern int errno; 
void perror();

#endif  /* __LIBC_H__ */
