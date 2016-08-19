#include <fcntl.h>
#include <stdio.h>
//#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int main()
{
  int fifo_fd[2];
  int pid, status;

  // open two FIFOs: fifo_cmd, fifo_ret
  // note: the FIFOs have to be created in advance.
  fifo_fd[0] = open("fifo_cmd", O_RDONLY); // for VDMJ to receive commands
  fifo_fd[1] = open("fifo_ret", O_WRONLY); // for VDMJ to return results
  //duplicate fifo_cmd stdin
  dup2(fifo_fd[0],0);
  //Duplicate fifo_ret to stdout
  dup2(fifo_fd[1],1);
  //Duplicate fifo_ret to stderr
  dup2(fifo_fd[1],2);

  while (1) {
    pid = fork();
    if (pid == -1) fprintf(stderr, "Error: fork() returns -1\n");
    if (pid == 0) { // child
      // start VDMJ
      write(fifo_fd[1], "\n", strlen("\n")); // need to write something before start VDMJ or the FIFOs won't transfer.
      //execlp("java", "java", "-jar", "vdmj-3.0.1.jar", "-vdmsl", "counter.vdmsl", "-i", NULL);
      //execlp("java", "java", "-jar", "vdmj-3.0.1.jar", "-vdmpp", "counter.vdmpp", "-i", NULL);
      execlp("java", "java", "-jar", "vdmj-3.0.1.jar", "-vdmsl", "vendingmachine.vdmsl", "IO.vdmsl", "-default", "verifier", "-i", NULL);  //remember to set default module to SAFER
      return 0;
    }
    else { // parent
      waitpid(pid, &status, 0);
    }
  }

  // finishing (should never be here :-) )
  printf("fifo_safer is finishing...\n");
  close(fifo_fd[0]); close(fifo_fd[1]);
  return 1;
}
