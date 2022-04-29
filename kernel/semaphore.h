#include "spinlock.h"
 
// Counting semaphore
struct semaphore {
    struct spinlock lock;      // semaphore lock
    int    count;                   // semaphore value
    int    valid;
};

