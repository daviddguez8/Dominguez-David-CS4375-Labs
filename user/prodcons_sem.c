#include "kernel/types.h"
#include "kernel/stat.h"
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

void *producer(void *arg)
{
  while(1) {
    sem_wait(&buffer->free);
    sem_wait(&buffer->lock);
    if (buffer->num_produced >= MAX) {
        sem_post(&buffer->free);
        sem_post(&buffer->occupied);
        sem_post(&buffer->lock);
        pthread_exit(0);
    }
    buffer->num_produced++;
    printf("producer %lld producing %d\n", (long long int) arg, buffer->num_produced);
    buffer->buf[buffer->nextin++] = buffer->num_produced;

    buffer->nextin %= BSIZE;
    sem_post(&buffer->occupied);

    /* Now either buffer->occupied < BSIZE and buffer->nextin is the index
       of the next empty slot in the buffer, or
       buffer->occupied == BSIZE and buffer->nextin is the index of the
       next (occupied) slot that will be emptied by a consumer
       (such as buffer->nextin == buffer->nextout) */

    sem_post(&buffer->lock);
  }
}

void *consumer(void *arg)
{
  while(1) {
    sem_wait(&buffer->occupied);
    sem_wait(&buffer->lock);
    if (buffer->num_consumed >= MAX) {
        sem_post(&buffer->occupied);
        sem_post(&buffer->free);
        sem_post(&buffer->lock);
        pthread_exit(0);
   }
    printf("consumer %lld consuming %d\n", (long long int) arg, buffer->buf[buffer->nextout]);
    buffer->total += buffer->buf[buffer->nextout++];
    buffer->nextout %= BSIZE;
    buffer->num_consumed++;
    sem_post(&buffer->free);

    /* now: either b->occupied > 0 and b->nextout is the index
       of the next occupied slot in the buffer, or
       b->occupied == 0 and b->nextout is the index of the next
       (empty) slot that will be filled by a producer (such as
       b->nextout == b->nextin) */

    sem_post(&buffer->lock);

  }
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
     printf("usage: %s <nproducers> <nconsumers>\n", argv[0]);
     return -1;
  }
  int nproducers = atoi(argv[1]);
  int nconsumers = atoi(argv[2]);
  pthread_t pid[nproducers], cid[nconsumers];
  long long int i;

  buffer = (buffer_t *) malloc(sizeof(buffer_t));
  buffer->nextin = 0;
  buffer->nextout = 0;
  buffer->num_produced = 0;
  buffer->num_consumed = 0;
  buffer->total = 0;
  printf("sizeof(buffer->lock): %ld\n", sizeof(buffer->lock));
  sem_init(&buffer->occupied, 0, 0);
  sem_init(&buffer->free, 0, BSIZE);
  sem_init(&buffer->lock, 0, 1);

  for (i = 0; i < nconsumers; i++)
    pthread_create(&cid[i], NULL, consumer, (void *)i);
  for (i = 0; i < nproducers; i++)
    pthread_create(&pid[i], NULL, producer, (void *)i);
  for (i = 0; i < nconsumers; i++)
    pthread_join(cid[i], NULL);
  for (i = 0; i < nproducers; i++)
    pthread_join(pid[i], NULL);
  printf("total = %d\n", buffer->total);

  return 0;
}
