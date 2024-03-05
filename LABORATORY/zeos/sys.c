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

/**
 * Writes data to a file descriptor.
 *
 * This function writes data from the specified buffer to the file associated with the file descriptor.
 * The amount of data written may be less than requested if there is insufficient space available.
 *
 * @param fd The file descriptor to write to.
 * @param buffer Pointer to the buffer containing the data to be written.
 * @param size The number of bytes to write.
 * @return Upon successful completion, the number of bytes written is returned. On error, -1 is returned,
 * and errno is set appropriately to indicate the error.
 */
int sys_write(int fd, char * buffer, int size){

  unsigned long bytes_written = 0;
  
  // Check the file descriptor
  int fd_check = check_fd(fd, ESCRIPTURA);
  if (fd_check < 0)   return fd_check;

  // Check if buffer is not null
  if (buffer == NULL) return -14; /* EFAULT */

  // Check size is positive
  if (size < 0)       return -22; /* EINVAL */

  ///// TO DO

  return bytes_written;
}

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

