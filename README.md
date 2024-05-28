# ZeOS Operating System
## Description
ZeOS is a basic operating system Kernel based on UNIX and POSIX which implements boot process, syscalls, interruptions, exceptions and processes. In order to make use of the capabilities of this kernel,
the user can edit the **user.c** file which has a **lib.c** library that includes utilities and systemc all wrappers. 

This kernel provides a range of system calls that enable various functionalities from process management to interacting with the screen and shared memory. Below is a summary of all the available system calls:

### Process Management

- **exit**: Terminates the calling process.
- **fork**: Creates a new process by duplicating the calling process.
- **getpid**: Returns the process ID of the calling process.
- **numsons**: Returns the number of child processes of the calling process.
- **numbros**: Returns the number of sibling processes of the calling process.

### Input/Output Operations

- **read**: Reads data from a circular buffer keylogger.
- **write**: Writes data to the screen.

### Screen Management

- **gotoxy**: Moves the cursor to a specified position on the screen.
- **set_color**: Changes the background and foreground font color on the screen.

### Time Management

- **gettime**: Retrieves the current system time.

### Shared Memory Operations

- **shmat**: Attaches a shared memory page to a logical page in the process memory.
- **shmdt**: Detaches a previously attached shared memory page.
- **shmrm**: Zeros out a shared memory page.

### Process Control

- **block**: Blocks the calling process, preventing it from executing.
- **unblock**: Unblocks a specified process, allowing it to execute.

Each of these system calls provides essential functionalities to manage processes, handle input/output operations, interact with the screen, and manage shared memory in a multitasking environment. 

## Usage 
The kernel must be compiled and then booted. We recommend execution on the bochs virtual machine:

```console
sudo apt-get install build-essential
sudo apt-get install bin86
apt-get -y install libx11-dev
apt-get -y install libgtk2.0-dev
apt-get -y install libreadline-dev
```
```
Download BOCHS from source and unzip it:
./configure --enable-gdb-stub --with-x --prefix=/opt/bochs_gdb
make all install
```

After installing all the environment, the kernel can be easily compiled using the Makefile provided and executed through bochs:
```console
cd UPC-SOA/zeos
make emul
```

To execute the program using GDB, we provide ```make gdb``` in the Makefile.

## License
This project is licensed under the GNU General Public License (GPL) version 3.0, allowing you to freely use, modify, and distribute this software under the terms of the GPL.
