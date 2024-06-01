/*
 * libc.h - macros per fer els traps amb diferents arguments
 *          definició de les crides a sistema
 */
 
#ifndef __LIBC_H__
#define __LIBC_H__

//string utils
void itodeca(int a, char *b);
void itohexa(int a, char *b);
int strlen(char *a);
void strcat(char *dest, char* src);
void strcpy(char *dest, char* src);

//Psuedo-RNG utils following a Linear Congruential Generator (LCG)
//X_n+1 = (A * X_n + C) mod M

#define LCG_A 1664525 
#define LCG_C 1013904223
#define LCG_M 4294967296

void srand(unsigned int seed);
unsigned int rand();
unsigned int rand_range(unsigned int min, unsigned int max);


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
