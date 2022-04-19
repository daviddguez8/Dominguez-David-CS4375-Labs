#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
piping_primes(int p) {
  int new_pipe[2];
  pipe(new_pipe);
  
  //need to read from pipe and pass to another pipe if they are prime
  int my_prime;
  int keep_going=1;
  read(p, &my_prime, sizeof(int));
  printf("prime %d\n", my_prime);

  int temp = -1;
  //we want to eliminate all numbers multiple of 2
  while(keep_going==1){
    //read the next available number
    int read_result = read(p, &temp, sizeof(int));

    if(read_result <= 0) break;

    //multiple of this prime, skip
    if (temp % my_prime == 0) continue;

    write(new_pipe[1], &temp, sizeof(int));
    
  }
  //this child didn't read anything, means we reached the end
  if(temp == -1) {
    close(new_pipe[0]);
    close(new_pipe[1]);
    close(p);
    exit(0);
  }
  
  int pid = fork();//create a new process

  if(pid == 0) {
    //child
    close(new_pipe[1]);//wont write to this pipe anymore
    close(p);
    piping_primes(new_pipe[0]);
    close(new_pipe[0]);
    
  }
  else {
    //parent
    close(new_pipe[0]);
    close(new_pipe[1]);
    close(p);
    wait(0);//wait for children to finish
    
    
  }
  exit(0);
}


int
main(int argc, char *argv[]) {
  
  int p[2];
  pipe(p);
  //write all the numbers from 2 to 35
  for(int i = 2; i<=35; i++) {
    write(p[1], &i, sizeof(int));
  }

  //don't need the write end of the pipe anymore
  close(p[1]);
  //call the piping system using the read end
  piping_primes(p[0]);

  close(p[0]);
  exit(0);
  
}
