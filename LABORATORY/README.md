# ZeOS Operating System
## Description
ZeOS is a basic operating system Kernel based in UNIX and POSIX which implements boot process, syscalls, interruptions, exceptions and processes. In order to make use of the capabilities of this kernel,
the user can edit the **user.c** file which has a **lib.c** library that includes utilities and systemc all wrappers. The list of implemented syscalls are:

- write
- gettime
- fork
- exit 
- getpid
- numbros (Get the number of brothers of the current process)
- numsons (Get the number of sons of the current process)
- block (Blocks the current process)
- unblock (Unblock a children process)

## Usage 
The kernel must be compiled and then booted, therefore it can't be executed on a running computer, there are mainly two options:
1. Using an MBR x86 bootable device, such as an USB (and configure the BIOS/EFI to use this device as the first boot device).
2. Executing it on a virtual machine, such as *BOCHS*

For the later option, one must install the build essentials (gcc, gdb, ld ...), x86 tools and manually install bochs
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
cd UPC-SOA/LABORATORY/zeos
make emul
```

To execute the program using GDB, we provide ```make gdb``` in the Makefile.

## Creators
- Walter Troiani
- Alexandre Ros

Credits to Alex Pajuelo (Our "beloved" teacher of this subject) and to all the contributors that made ZeOS possible
 
## To be Implemented
In a near future the docker image of ZeOS will be provided so the user can use the bochs interface easier without having to manually install BOCHS. Also some new features and syscalls are on the way:
- [ ] getppid syscall
- [ ] wait syscall
- [ ] waitpid syscall
- [ ] exec syscall 

## License
This project is licensed under the GNU General Public License (GPL) version 3.0, allowing you to freely use, modify, and distribute this software under the terms of the GPL.
