#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
find(char *path, char *target)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  //opens the given path
  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  //gets its info
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  //determines what the given path was
  switch(st.type){
    //if it was just a file, we can print it as in ls before
  case T_FILE:
    //if (strcmp(p, target) == 0) {//it is a file, so compare it to target, if match print
    //        printf("%s %d %d %d\n", buf, st.type, st.ino, st.size);
    //}
    //printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      //we want to check two things
      //if the just opened fd is a directory
      //recursively call find if dir_name starts with a . (for . and ..)
      if(st.type == T_DIR && *p != '.') {
        find(buf, target);
      }
      else if (strcmp(p, target) == 0) {//it is a file, so compare it to target, if match print
        printf("%s %d %d %d\n", buf, st.type, st.ino, st.size);
      }
       
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
 
  if(argc < 2){
    printf("Usage: find <dir_name> <file_name>...\n");
    exit(1);
  }
  
  find(argv[1], argv[2]);
  exit(0);
}
