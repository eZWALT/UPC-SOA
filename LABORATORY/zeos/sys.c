/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

void sys_exit()
{  
}

//System gettime -- 
int sys_gettime(){

}

//System write -- Returns negative number in case of error, specifying the error code. Otherwise the number of bytes written
int sys_write(int fd, void* buffer, size_t size){
  unsigned int bytes_written = 0;
  
  //Checks 
  int fd_error = check_fd(fd, ESCRIPTURA);
  if(fd_error < 0) return fd_error;
  if(buffer == NULL) return  -14; /*EFAULT*/
  if(size < 0) return -22; /*EINVAL*/

  //DO SMTH


  return bytes_written;
}