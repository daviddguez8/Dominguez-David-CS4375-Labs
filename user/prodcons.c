#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#define BUF_SIZE 10

int *buffer;

void producer() {
    for (int i=0; i<BUF_SIZE; i++) {
        
        buffer[i] = i;
        printf("parent assigning address %p, read %d\n", &buffer[i], buffer[i]);
    }
    return;
}

int consumer() {
    int sum = 0;
    for (int i=0; i<BUF_SIZE; i++) {
        printf("child reading address: %p, read %d\n", &buffer[i], buffer[i]);
        sum += buffer[i];
    }
        
    return(sum);
}

int main() {
    printf("it runs...\n");
    buffer = (int *) mmap(0, BUF_SIZE*sizeof(int), PROT_READ | PROT_WRITE, 
                          MAP_ANONYMOUS | MAP_SHARED, -1, 0);


    producer();
    if (!buffer) {
        printf("Error: mmap() failed\n");
        exit(-1);
    }

    printf("it maps... mapped to: %p\n", buffer);
    

    printf("lets fork\n");
    int rc = fork();
    if (!rc) {
        producer();
        exit(0);
    }
    else if (rc>0) {
        wait(0);
        printf("main consumed sum = %d\n", consumer());
    } else {
        printf("Error: fork() failed\n");
        exit(-1);
    }
    exit(0);
}
    