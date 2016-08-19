#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#define MAX_BUF 65535
#define BUF 2048
#define MIN_BUF 128

// boolean type
typedef enum { true=1, false=0, True=1, False=0, T=1, F=0 } bool;

// global variables
int fifo_fd[2]={-1,-1}, nbytes=-1, i=0, temp=0;
char readbuffer[BUF], writebuffer[BUF], strbuffer[MAX_BUF];
//int   pfd_c2p[2], pfd_p2c[2], nbytes=-1, i=0;
//pid_t childpid;
//char  readbuffer[BUF], writebuffer[BUF];
