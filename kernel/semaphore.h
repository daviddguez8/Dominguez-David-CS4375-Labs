// Counting semaphore
struct semaphore {
    struct spinlock lock;      // semaphore lock
    int    count;                   // semaphore value
    int    valid;
};

//table methods
void seminit(void);
uint64 semalloc(void);
int semdealloc(uint64 s_idx);

//per-semaphore methods
int sem_init(sem_t sem, int pshared, int value);
int sem_destroy(sem_t sem);
int sem_wait(sem_t sem);
int sem_post(sem_t sem);