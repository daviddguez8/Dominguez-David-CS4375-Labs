#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "semaphore.h"
#include "proc.h"


struct semtab{
    struct spinlock lock;
    struct semaphore sem[NSEM];
} semtable;

void
seminit(void)
{
    initlock(&semtable.lock, "semtable");
    for (int i = 0; i < NSEM; i++)
        initlock(&semtable.sem[i].lock, "sem");
}

uint64
semalloc(void) {
    acquire(&semtable.lock);

    uint64 s_idx;
    for (s_idx = 0; s_idx < NSEM; s_idx++) {
        if(semtable.sem[s_idx].valid == 0) {
            //found a free spot
            semtable.sem[s_idx].valid = 1;
            release(&semtable.lock);
            return s_idx;
        }
    }
    //didn't find a free semaphore spot
    release(&semtable.lock);
    return -1;
}

int
semdealloc(uint64 s_idx) {
    acquire(&semtable.lock);
    if(semtable.sem[s_idx].valid != 1) {
        release(&semtable.lock);
        return -1;
    }
    semtable.sem[s_idx].valid = 0;
    release(&semtable.lock);
    return 0;
}


int 
sem_init(sem_t sem, int pshared, int value) {
    printf("called");
    struct proc *p = myproc();

    uint64 available_idx = semalloc();

    if(available_idx < 0) return -1;

    semtable.sem[available_idx].count = value;
    
    if(copyout(p->pagetable, sem, (char*)&available_idx, sizeof(uint64)) < 0) 
    {
        semdealloc(available_idx);
        return -1;
    }
    return -1;
}

int 
sem_destroy(sem_t sem) {
    struct proc *p = myproc();

    uint64 sem_idx;
    copyin(p->pagetable, (char*)&sem_idx, sem, sizeof(uint64));
    
    acquire(&semtable.sem[sem_idx].lock);
    if(semdealloc(sem_idx) != 1) {
        printf("Couldn't deallocate semaphore at address %p\n", sem);
        release(&semtable.sem[sem_idx].lock);
        return -1;
    }
    release(&semtable.sem[sem_idx].lock);
    return -1;
}

//from OSTEP book:
//decrement the value of semaphore s by one 
// wait if value of semaphore s is < 0
int 
sem_wait(sem_t sem) {

    struct proc *p = myproc();

    uint64 sem_idx;
    copyin(p->pagetable, (char*)&sem_idx, sem, sizeof(uint64));

    if(semtable.sem[sem_idx].valid != 1) {
        printf("Semaphore at address %p not valid", sem);
        return -1;
    } 

    acquire(&semtable.sem[sem_idx].lock);
    while(semtable.sem[sem_idx].count <= 0) 
        sleep((void*)&semtable.sem[sem_idx], &semtable.sem[sem_idx].lock);
    semtable.sem[sem_idx].count-=1;
    release(&semtable.sem[sem_idx].lock);

    return -1;
}

//increment the value of semaphore s by one
//if there are one or more threads waiting, wake one
int 
sem_post(sem_t sem) {
    struct proc *p = myproc();

    uint64 sem_idx;
    copyin(p->pagetable, (char*)&sem_idx, sem, sizeof(uint64));

    if(semtable.sem[sem_idx].valid != 1) {
        printf("Semaphore at address %p not valid", sem);
        return -1;
    } 

    acquire(&semtable.sem[sem_idx].lock);
    semtable.sem[sem_idx].count+=1;
    wakeup(sem);
    release(&semtable.sem[sem_idx].lock);

    return -1;
}