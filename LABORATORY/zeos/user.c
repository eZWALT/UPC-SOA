#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
main(void)
{
    // Please modify this function with your desired user.c code
    test_task_switch();
    // NOP
    while(1);
}

inline void test_create_fork(){
    int pid = fork();

    char * msg = "\nChild PID: ";
    char buff[64];
    itodeca(pid, buff);
    
    write(1, msg, strlen(msg));
    write(1, buff, strlen(buff));
}

inline void test_two_forks(){
    int pid1 = fork();
    int pid2 = fork();

    char * msg1 = "\nChild 1 PID: ";
    char * msg2 = "\nChild 1 PID: ";

    char buff2[64];
    char buff1[64];

    itodeca(pid2, buff2);
    itodeca(pid1, buff1);

    write(1, msg1, strlen(msg1));
    write(1, buff1, strlen(buff1));
    write(1, msg2, strlen(msg2));
    write(1, buff2, strlen(buff2));

}

inline void test_task_switch(){

    char * msg = "\nI'm INIT! (aka TASK1)";

    write(1, msg, strlen(msg));

    // Press any key to perform the task_switch
    // Check that the msg is printed every time we switch from IDLE to INIT
}