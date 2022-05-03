#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/semaphore.h"
#include "user/user.h"

#define BSIZE 10
#define MAX 20

typedef struct {
    int buf[BSIZE];
    int nextin;
    int nextout;
    int num_produced;
    int num_consumed;
    int total;
    sem_t occupied;
    sem_t free;
    sem_t lock;
} buffer_t;

buffer_t *buffer;

void *producer()
{
  while(1) {
//    printf("producer calling sem_wait\n");
    sem_wait(buffer->free);
    sem_wait(buffer->lock);
    if (buffer->num_produced >= MAX) {
        sem_post(buffer->free);
        sem_post(buffer->occupied);
        sem_post(buffer->lock);
        exit(0);
    }
    buffer->num_produced++;
    printf("producer %d producing %d\n", getpid(), buffer->num_produced);
    buffer->buf[buffer->nextin++] = buffer->num_produced;

    buffer->nextin %= BSIZE;
    sem_post(buffer->occupied);

    /* Now either buffer->occupied < BSIZE and buffer->nextin is the index
       of the next empty slot in the buffer, or
       buffer->occupied == BSIZE and buffer->nextin is the index of the
       next (occupied) slot that will be emptied by a consumer
       (such as buffer->nextin == buffer->nextout) */

    sem_post(buffer->lock);
  }
}

void *consumer()
{
  while(1) {
 //   printf("Consumer calling sem_wait\n");
    sem_wait(buffer->occupied);
    sem_wait(buffer->lock);
    if (buffer->num_consumed >= MAX) {
        sem_post(buffer->occupied);
        sem_post(buffer->free);
        sem_post(buffer->lock);
        exit(0);
   }
    printf("consumer %d consuming %d\n", getpid(), buffer->buf[buffer->nextout]);
    buffer->total += buffer->buf[buffer->nextout++];
    buffer->nextout %= BSIZE;
    buffer->num_consumed++;
    sem_post(buffer->free);

    /* now: either b->occupied > 0 and b->nextout is the index
       of the next occupied slot in the buffer, or
       b->occupied == 0 and b->nextout is the index of the next
       (empty) slot that will be filled by a producer (such as
       b->nextout == b->nextin) */

    sem_post(buffer->lock);

  }
}

//test 1
//testing in the scenario of a single producer/consumer
int test1() {
    int nproducers = 1;
    int nconsumers = 1;
    int i;

    buffer = (buffer_t *) mmap(0, BSIZE*sizeof(buffer_t), 
                            PROT_READ | PROT_WRITE,
                            MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    buffer->nextin = 0;
    buffer->nextout = 0;
    buffer->num_produced = 0;
    buffer->num_consumed = 0;
    buffer->total = 0;
    printf("Calling sem_init on &buffer->occupied\n");
    sem_init(buffer->occupied, 1, 0);
    sem_init(buffer->free, 1, BSIZE);
    sem_init(buffer->lock, 1, 1);

    for (i = 0; i < BSIZE; i++)
        buffer->buf[i] = 0;
        
    
    for (i = 0; i < nconsumers; i++)
        if (!fork()) { 
            consumer();
            exit(0);    
        }
    
    printf("error not above\n");
    for (i = 0; i < nproducers; i++)
        if (!fork()) {
            producer();
            exit(0);
        }
    
    for (i = 0; i < nconsumers; i++)
        wait(0);
    for (i = 0; i < nproducers; i++)
        wait(0);

    printf("total = %d\n", buffer->total);
    sem_destroy(buffer->occupied);
    sem_destroy(buffer->free);
    sem_destroy(buffer->lock);
    munmap((void *)buffer, sizeof(buffer_t));
    return 1;
}

//test 2
//testing scenario of multiple producer and a single consumer
int test2() {
    int nproducers = 10;
    int nconsumers = 1;
    int i;

    buffer = (buffer_t *) mmap(0, BSIZE*sizeof(buffer_t), 
                            PROT_READ | PROT_WRITE,
                            MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    buffer->nextin = 0;
    buffer->nextout = 0;
    buffer->num_produced = 0;
    buffer->num_consumed = 0;
    buffer->total = 0;
    printf("Calling sem_init on &buffer->occupied\n");
    sem_init(buffer->occupied, 1, 0);
    sem_init(buffer->free, 1, BSIZE);
    sem_init(buffer->lock, 1, 1);

    for (i = 0; i < BSIZE; i++)
        buffer->buf[i] = 0;
        
    
    for (i = 0; i < nconsumers; i++)
        if (!fork()) { 
            consumer();
            exit(0);    
        }
    
    printf("error not above\n");
    for (i = 0; i < nproducers; i++)
        if (!fork()) {
            producer();
            exit(0);
        }
    
    for (i = 0; i < nconsumers; i++)
        wait(0);
    for (i = 0; i < nproducers; i++)
        wait(0);

    printf("total = %d\n", buffer->total);
    sem_destroy(buffer->occupied);
    sem_destroy(buffer->free);
    sem_destroy(buffer->lock);
    munmap((void *)buffer, sizeof(buffer_t));
    return 1;
}

//test 3
//testing the scenario of multiple consumers and a single producer
int test3() {
    int nproducers = 1;
    int nconsumers = 10;
    int i;

    buffer = (buffer_t *) mmap(0, BSIZE*sizeof(buffer_t), 
                            PROT_READ | PROT_WRITE,
                            MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    buffer->nextin = 0;
    buffer->nextout = 0;
    buffer->num_produced = 0;
    buffer->num_consumed = 0;
    buffer->total = 0;
    printf("Calling sem_init on &buffer->occupied\n");
    sem_init(buffer->occupied, 1, 0);
    sem_init(buffer->free, 1, BSIZE);
    sem_init(buffer->lock, 1, 1);

    for (i = 0; i < BSIZE; i++)
        buffer->buf[i] = 0;
        
    
    for (i = 0; i < nconsumers; i++)
        if (!fork()) { 
            consumer();
            exit(0);    
        }
    
    printf("error not above\n");
    for (i = 0; i < nproducers; i++)
        if (!fork()) {
            producer();
            exit(0);
        }
    
    for (i = 0; i < nconsumers; i++)
        wait(0);
    for (i = 0; i < nproducers; i++)
        wait(0);

    printf("total = %d\n", buffer->total);
    sem_destroy(buffer->occupied);
    sem_destroy(buffer->free);
    sem_destroy(buffer->lock);
    munmap((void *)buffer, sizeof(buffer_t));
    return 1;
}

//test 4
//testing the scenario of multiple producers/consumers
int test4() {
    int nproducers = 9;
    int nconsumers = 8;
    int i;

    buffer = (buffer_t *) mmap(0, BSIZE*sizeof(buffer_t), 
                            PROT_READ | PROT_WRITE,
                            MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    buffer->nextin = 0;
    buffer->nextout = 0;
    buffer->num_produced = 0;
    buffer->num_consumed = 0;
    buffer->total = 0;
    printf("Calling sem_init on &buffer->occupied\n");
    sem_init(buffer->occupied, 1, 0);
    sem_init(buffer->free, 1, BSIZE);
    sem_init(buffer->lock, 1, 1);

    for (i = 0; i < BSIZE; i++)
        buffer->buf[i] = 0;
        
    
    for (i = 0; i < nconsumers; i++)
        if (!fork()) { 
            consumer();
            exit(0);    
        }
    
    printf("error not above\n");
    for (i = 0; i < nproducers; i++)
        if (!fork()) {
            producer();
            exit(0);
        }
    
    for (i = 0; i < nconsumers; i++)
        wait(0);
    for (i = 0; i < nproducers; i++)
        wait(0);

    printf("total = %d\n", buffer->total);
    sem_destroy(buffer->occupied);
    sem_destroy(buffer->free);
    sem_destroy(buffer->lock);
    munmap((void *)buffer, sizeof(buffer_t));
    return 1;
}



int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("usage: %s <nproducers> <nconsumers>\n", argv[0]);
        return -1;
    }
    test1();
    test2();
    test3();
    test4();
    exit(0);
}