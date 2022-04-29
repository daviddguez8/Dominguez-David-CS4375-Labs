#include "defs.h"
#include "spinlock.h"
#include "semaphore.h"
#include "param.h"

struct {
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