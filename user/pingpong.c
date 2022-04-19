#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
  
  char buf[2];
  buf[0] = 'X';
  buf[1] = '0';
  int p[2];
  
  pipe(p);
  if(fork() == 0) {
    //child process
    //receive byte in argv[1]
    
    //fprintf(1, "Byte to be sent: ");
    //printf(argv[1]);
    //printf("\n");
    
    printf("%d: received ping\n", getpid());
    close(p[0]); 
    //write the byte (<pid>) on the pipe to the parent
    write(p[1], buf, 1);
    close(p[1]);
    exit(0);
  }
  else {
    wait(0);
    //parent process
    //read the byte from the children
    char byte[2];
    close(p[1]);
    read(p[0], byte, 1);
    
    //fprintf(1, "Byte read: ");
    //printf("%c\n", byte[0]);

    printf("%d: received pong\n", getpid());
    close(p[0]);
    exit(0);
  }
  exit(0);
  
}
