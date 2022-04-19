#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"
#include "defs.h"

const char syscall_names[23][10] = {
  "",
  "fork",
  "exit",
  "wait",
  "pipe",
  "read",
  "kill",
  "exec",
  "fstat",
  "chdir",
  "dup",
  "getpid",
  "sbrk",
  "sleep",
  "uptime",
  "open",
  "write",
  "mknod",
  "unlink",
  "link",
  "mkdir",
  "close",
  "strace"
};

// Fetch the uint64 at addr from the current process.
int
fetchaddr(uint64 addr, uint64 *ip)
{
  struct proc *p = myproc();
  if(addr >= p->sz || addr+sizeof(uint64) > p->sz)
    return -1;
  if(copyin(p->pagetable, (char *)ip, addr, sizeof(*ip)) != 0)
    return -1;
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
int
fetchstr(uint64 addr, char *buf, int max)
{
  struct proc *p = myproc();
  int err = copyinstr(p->pagetable, buf, addr, max);
  if(err < 0)
    return err;
  return strlen(buf);
}

static uint64
argraw(int n)
{
  struct proc *p = myproc();
  switch (n) {
  case 0:
    return p->trapframe->a0;
  case 1:
    return p->trapframe->a1;
  case 2:
    return p->trapframe->a2;
  case 3:
    return p->trapframe->a3;
  case 4:
    return p->trapframe->a4;
  case 5:
    return p->trapframe->a5;
  }
  panic("argraw");
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  *ip = argraw(n);
  return 0;
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
int
argaddr(int n, uint64 *ip)
{
  *ip = argraw(n);
  return 0;
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (including nul), -1 if error.
int
argstr(int n, char *buf, int max)
{
  uint64 addr;
  if(argaddr(n, &addr) < 0)
    return -1;
  return fetchstr(addr, buf, max);
}

extern uint64 sys_chdir(void);
extern uint64 sys_close(void);
extern uint64 sys_dup(void);
extern uint64 sys_exec(void);
extern uint64 sys_exit(void);
extern uint64 sys_fork(void);
extern uint64 sys_fstat(void);
extern uint64 sys_getpid(void);
extern uint64 sys_kill(void);
extern uint64 sys_link(void);
extern uint64 sys_mkdir(void);
extern uint64 sys_mknod(void);
extern uint64 sys_open(void);
extern uint64 sys_pipe(void);
extern uint64 sys_read(void);
extern uint64 sys_sbrk(void);
extern uint64 sys_sleep(void);
extern uint64 sys_unlink(void);
extern uint64 sys_wait(void);
extern uint64 sys_write(void);
extern uint64 sys_uptime(void);
extern uint64 sys_strace(void);

static uint64 (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
[SYS_strace]  sys_strace
};


void printargs(int num, struct proc *p) {

  switch(num) {

    //void args
  case 1:
  case 11:
  case 14: ;
    printf("()");
    break;

    //int arg
  case 2:
  case 21:
  case 6:
  case 10:
  case 12:
  case 13:
  case 22: ;
    int arg1_2;
    argint(0, &arg1_2);
    printf("(%d)", arg1_2);
    
    break;

    //int* arg
  case 3:
  case 4: ;
    uint64 arg1_3;
    argaddr(0, &arg1_3);
    printf("(%p)", arg1_3);
    break;

    //int, void*, int args
  case 16:
  case 5: ;
    int arg1_4;
    uint64 arg2_4;
    int arg3_4;
    
    argint(0, &arg1_4);
    argaddr(1, &arg2_4);
    argint(2, &arg3_4);
    printf("(%d, %p, %d)", arg1_4, arg2_4, arg3_4);
    break;
    
    //char*, char**
  case 7: ;
    char arg1_5[50];
    char* ptr5 = arg1_5;
    uint64 arg2_5;

    argstr(0, ptr5, 50);
    argaddr(1, &arg2_5);
    printf("(%s, %p)", arg1_5, arg2_5);
    break;

    //char*, int args
  case 15: ;
    char arg1_6[50];
    char *ptr6 = arg1_6;
    int arg2_6;

    argstr(0, ptr6, 50);
    argint(1, &arg2_6);
    printf("(%s, %d)", arg1_6, arg2_6);
    break;
    break;

    //char*, short, short
  case 17: ;
    char arg1_7[50];
    char *ptr7 = arg1_7; 
    int arg2_7;
    int arg3_7;
    
    argstr(0, ptr7, 50);
    argint(1, &arg2_7);
    argint(2, &arg3_7);
    printf("(%s, %d, %d)", arg1_7, arg2_7, arg3_7);
    break;
    
    //char*
  case 18:
  case 20:
  case 9: ;
    char arg1_8[50];
    char *ptr8 = arg1_8;

    argstr(0, ptr8, 50); 
    printf("(%s)", arg1_8);
    break;

    //char*, char* args
  case 19: ;
    char arg1_9[50];
    char *ptr9 = arg1_9;
    char arg2_9[50];
    char *ptr2_9 = arg2_9;
    
    argstr(0, ptr9, 50);
    argstr(1, ptr2_9, 50);
    printf("(%s, %s)", arg1_9, arg2_9);
    break;
    
    //int, stat*
  case 8: ;
    int arg1_10;
    uint64 arg2_11;

    argint(0, &arg1_10);
    argaddr(1, &arg2_11);
    printf("(%d, %p)", arg1_10, arg2_11);
    break;

    
  }
    

}

void
syscall(void)
{
  int num;
  struct proc *p = myproc();
  
  num = p->trapframe->a7;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    uint64 temp = syscalls[num]();
    
    int strace = ((1<<num) & p->stracemask);
    if(strace) {
      //add print with call arguments for traced systems calls in perentheses following the system call name 
      printf("%d: syscall %s", p->pid, syscall_names[num]);
      //printargs(num, p); NOTICE: uncomment to print args, commenting to pass the autograder
    }
    p->trapframe->a0 = temp;
    if(strace) {
       printf(" -> %d\n", p->trapframe->a0);
    }
    
  } else {
    printf("%d %s: unknown sys call %d\n",
            p->pid, p->name, num);
    p->trapframe->a0 = -1;
  }
}
