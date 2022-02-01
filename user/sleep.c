#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  
  if(argc < 2){
    fprintf(2, "Usage: sleep time...\n");
    exit(1);
  }

  char* sleep_time = argv[1];

  sleep(atoi(sleep_time));
  exit(0);
}
