#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
main(void)
{
    // Please modify this function with your desired user.c code
    test_scheduling();
    // NOP
    while(1){}
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

inline void test_scheduling(){
    char buff[16];
    char * nl = "\n PID = ";

    fork();
    fork();

    int my_pid = getpid();
    itodeca(my_pid, buff);

    while (1){
        write(1, nl, strlen(nl));
        write(1, buff, strlen(buff));

    }
}

inline void test_scheduling_2()
{
    char * msg = "\nAlive";

    int x = fork();

    if (x == 0) while(1){ write(1, msg, strlen(msg)); }
    else exit();
}