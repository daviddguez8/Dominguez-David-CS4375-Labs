#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

void
clean_after(char **params, int i_params) {
  while(i_params < MAXARG) {
    params[i_params] = 0;
    i_params++;
  }
}

int
get_next_params(char **params, int i_params) {

  //int i_ret = 0;
  char ch;
  char *buf = malloc(128);
  int i_buf = 0;
  int read_something = 0;
  
  while(read(0, &ch, 1) > 0) {
    read_something = 1;
    if(ch == ' ' || ch == '\t' || ch == '\n'){
      //add current buf to ret_params
      buf[i_buf] = '\0';
      params[i_params] = buf;
      //printf("%s\n", ret_params[i_ret]);
      i_params++;
      i_buf = 0;
      buf = malloc(128);
      if(ch == '\n') break;
      continue;
    }
    buf[i_buf] = ch;
    i_buf++;
  }
  return read_something;
  //printf("%d\n", i_params);
  //printf("%s\n", params[0]);
  //for(int i = 0; i < i_params; i++) {
    //printf("%d: %s\n", i, params[i]);
  //}
  //return ret_params;
}

int
main(int argc, char *argv[])
{
  if(argc <= 1) {
    printf("Usage: xargs <cmd> <param1> <param2>...\n");
  }

  char *new_params[MAXARG];
  int params_i;
  for(params_i = 1; params_i < argc; params_i++) {
    new_params[params_i-1] = argv[params_i];
  }

  --params_i;
  while(get_next_params(new_params, params_i) != 0) {
    
    int pid = fork();

    if(pid == 0){
      exec(new_params[0], new_params);
    }
    wait(0);

    clean_after(new_params, params_i);
  }

  //get_next_params(new_params, params_i);
  //printf("\n");
  //printf("%s\n",buf);
    
  exit(0);
}
