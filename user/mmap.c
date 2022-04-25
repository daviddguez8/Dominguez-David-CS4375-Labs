#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

//void *addr, size_t length, int prot, int flags, int f, off_t offset
int
main(int argc, char[] argv) {
  if(argc <= 7) {
    printf("Usage: mmap <> <param1> <param2>...\n");
  }
  return 0;
} 
