/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>
#include <utils.h>
#include <io.h>
#include <mm.h>
#include <mm_address.h>
#include <sched.h>
#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1
#define MAX_BUFFER_SIZE 512


int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

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
int sys_write(int fd, char * buffer, int size)
{
  int ret = check_fd(fd, ESCRIPTURA);
  char buffer2[MAX_BUFFER_SIZE];
  int remaining_bytes = size;
  
  // Check the file descriptor
  if (ret)  return ret;

  // Check if buffer is not null
  if (!access_ok(VERIFY_READ, buffer, size)) return EFAULT; /* EFAULT */

  // Check size is positive
  if (size < 0)       return EINVAL; /* EINVAL */

  //Writing through console is done by chunks of 512 bytes each at a time 
  while(remaining_bytes > MAX_BUFFER_SIZE){
    //Copy 512 bytes (USER -> KERNEL)
    copy_from_user(buffer, buffer2, MAX_BUFFER_SIZE);
    //Write 512 bytes to console
    ret = sys_write_console(buffer2, MAX_BUFFER_SIZE);
    remaining_bytes -= ret;
    buffer += ret;
  }

  if(remaining_bytes > 0){
    copy_from_user(buffer,buffer2, remaining_bytes);
    ret = sys_write_console(buffer2, remaining_bytes);
    remaining_bytes -= ret;
  }  

  return size - remaining_bytes;
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
  return 14;
}

