/*
 * libc.c
 */

#include <errno.h>
#include <io.h>
#include <libc.h>
#include <types.h>

// Global variable for error codes
int errno;
unsigned int lcg_seed = 1000;

void perror() {
    char error_msg[256];
    // Errno is stored as an string into error_msg
    // itodeca(errno, error_msg);

    switch (errno) {
    case 0:
        strcpy(error_msg, "\nOK\n");
        break;
    case EBADF:
        strcpy(error_msg,
               "\nEBADF: The file descriptor is not valid, are you dumb?\n");
        break;
    case EACCES:
        strcpy(error_msg,
               "\nEACESS: You are not allowed to write this file you stupid\n");
        break;
    case EFAULT:
        strcpy(error_msg, "\nEFAULT: Bad address accessed, quit playing\n");
        break;
    case EINVAL:
        strcpy(error_msg, "\nEINVAL: invalid argument\n");
        break;
    case ENOSYS:
        strcpy(error_msg, "\nENOSYS: SuSCall not implemented (yet)\n");
        break;
    case ENOMEM:
        strcpy(error_msg, "\nENOMEM: Insufficient memory, download more ram\n");
        break;
    case ECHILD:
        strcpy(error_msg, "\nECHILD: We're childless\n");
        break;
    case ENOCOLOR:
        strcpy(error_msg, "\nENOCOLOR: Trying to set an invalid color! (4-bit "
                          "color scale)\n");
        break;
    case EOUTSCREEN:
        strcpy(error_msg, "\nEOUTSCREEN: Trying to set an invalid position! "
                          "(80 columns and 25 rows)\n");
        break;
    case ENOSHR:
        strcpy(error_msg, "\nENOSHR: Invalid shared page argument!\n");
        break;
    default:
        strcpy(error_msg, "\nUnknown error\n");
        break;
    }
    write(1, error_msg, strlen(error_msg));
}

char lower(char c){
    if(c >= 'A' && c <= 'Z'){
        return c + ('a' - 'A');
    }
    else return c;
}
void itodeca(int a, char *b) {
    int i, i1;
    char c;

    if (a == 0) {
        b[0] = '0';
        b[1] = 0;
        return;
    }

    i = 0;
    while (a > 0) {
        b[i] = (a % 10) + '0';
        a = a / 10;
        i++;
    }

    for (i1 = 0; i1 < i / 2; i1++) {
        c = b[i1];
        b[i1] = b[i - i1 - 1];
        b[i - i1 - 1] = c;
    }
    b[i] = 0;
}

void itohexa(int a, char *b) {
    int i, r;
    char c;

    if (a == 0) {
        b[0] = '0';
        b[1] = 0;
        return;
    }

    i = 0;
    while (a > 0) {
        r = a % 16;
        a = a / 16;

        if (r < 10)
            b[i] = '0' + r;
        else
            b[i] = 'A' + r - 10;

        ++i;
    }

    int i1;
    for (i1 = 0; i1 < i / 2; i1++) {
        c = b[i1];
        b[i1] = b[i - i1 - 1];
        b[i - i1 - 1] = c;
    }
    b[i] = 0;
}

/* Append SRC to the end of DEST */
void strcat(char *dest, char *src) {
    char *destPtr = dest + strlen(dest);
    while (*destPtr++ = *src++)
        ;
}

/* Replace SRC with DEST */
void strcpy(char *dest, char *src) {
    while (*dest++ = *src++)
        ;
}

int strlen(char *a) {
    int i;

    i = 0;

    while (a[i] != 0)
        i++;

    return i;
}

//Pseudo-RNG utils
void srand(unsigned int seed){
    lcg_seed = seed;
}
unsigned int rand(){
    lcg_seed = (LCG_A * lcg_seed + LCG_C) % LCG_M;
    return lcg_seed; 
}
unsigned int rand_range(unsigned int min, unsigned int max){
    return min + (rand() % (max - min + 1));
}

//Time utils
unsigned int get_time_ms(){
    return gettime() / MILISECONDS_PER_TICK;
}

unsigned int get_time_s(){
    return gettime() / SECONDS_PER_TICK;
}

void sleep(unsigned int ms){
    unsigned int start_time = gettime();
    unsigned int end_time = start_time + (ms / MILISECONDS_PER_TICK);
    while(gettime() < end_time);
}

unsigned int measure_time_ms(void (*f)()){
    unsigned int start_time = gettime();
    f();
    unsigned int end_time = gettime();
    return (end_time - start_time) * MILISECONDS_PER_TICK;
}