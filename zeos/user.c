#include <libc.h>
#include <list.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
main(void)
{
    // Please modify this function with your desired user.c code
    // NOP
    test_scheduling();
    while(1);
}

inline void test_childless(){
    unblock(6);
    perror();
}

inline void test_many_forks(){
    fork();
    fork();
    fork();
    fork();
    // This is too many forks
    perror();
}

inline void test_fork_bomb(){
    while(1)
    {
        fork();
        perror();
    }
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

inline void test_block(){
    int pid = fork();

    if (pid == 0) block();
    else {
        char * msg = "\n I'm the father and I'm going to unblock my son";
        write(1, msg, strlen(msg));

        for (unsigned int i = 0; i < 10000000; ++i)
        {
            // Doing nothing, but wasting time to let the child enter the block
            char * msg = "";
            write(1, msg, strlen(msg));
        }
        unblock(pid);
    }

    if(pid == 0){
        char * msg = "\nChild unblocked";
        write(1, msg, strlen(msg));
    }
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

inline void test_exit(){
    char * msg = "\nI'm about to EXIT!";
    write(1, msg, strlen(msg));
    exit();
}

inline void test_exit_2(){
    int son_pid = fork();
    if(son_pid == 0){
        son_pid = fork();
        if(son_pid != 0) exit();
    } 


    while(1);
}

inline void test_exit_3(){
    int pid1 = fork();
    if(pid1 != 0) pid1 = fork();

    if(pid1 != 0){
        char buff[32];
        itodeca(numsons(), buff);
        write(1, buff, strlen(buff));
    }
    else{
        char buff[32];
        char * msg = "\nI have x bros: ";
        itodeca(numbros(), buff);
        write(1, msg, strlen(msg));
        write(1, buff, strlen(buff));
        exit();

    }
    
}
