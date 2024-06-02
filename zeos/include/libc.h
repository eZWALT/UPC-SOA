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
char lower(char c);

//Psuedo-RNG utils following a Linear Congruential Generator (LCG)
//X_n+1 = (A * X_n + C) mod M

#define MULTIPLIER 1103515245
#define INCREMENT 12345
#define MODULUS 2147483648 // 2^31

void srand(unsigned int seed);
unsigned int rand();
unsigned int rand_range(unsigned int min, unsigned int max);

//Error utils
extern int errno; 
void perror();

//User time utils
#define TICKS_PER_SECOND 400
#define TICKS_PER_MILISECOND (1000 / TICKS_PER_SECOND)
unsigned int get_time_ms();
unsigned int get_time_s();
void sleep(unsigned int ms);
unsigned int measure_time_ms(void (*f)());

//Game syscalls wrappers
int gotoxy(int x, int y);
int set_color(int fg, int bg);

//Shared memory wrappers
void* shmat(int id, void* addr);
int shmdt(void* addr);
int shmrm(int id);

//Custom syscalls for current process
int numsons();
int numbros();

//I/O syscalls wrappers
int write(int fd, char *buffer, int size);
int read(char* b, int maxchars);


//Process management wrappers
int getpid();
int fork();
void exit();
void block();
int unblock(int pid);

//Time wrappers
int gettime();

#endif  /* __LIBC_H__ */
