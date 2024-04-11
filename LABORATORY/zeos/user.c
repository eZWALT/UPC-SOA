#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
  int pid = getpid();
  char * msg = "Alex ponos un 10 contaremos tu secreto al mundo\n PID: ";
  char buff[64];
  itodeca(pid, buff);
  write(1, msg, strlen(msg));
  write(1, buff, strlen(buff));
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  // Deliberately raises a page fault exception  
  //char * p = 0;
  //*p = "F";

  /*char * p = "Hallo welt";
  write(1, p, strlen(p));
  perror();*/

  /*long long i = 0;


  write(1, "\nShowing clock ticks since beginning:\n", 38); 

  while(1) {
    if (++i == 100000000) i = 0;

    if (i == 0){
      int get_time = gettime();
      char k[15];

      itodeca(get_time, k);


      write(1, k, strlen(k));
      write(1, "\n", 1);
    }
  }*/
	while(1);

  //NO FAGIS RETURN MAI!!!! (MALVAT)
}
