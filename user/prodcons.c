#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUF_SIZE 10

/* 
 * VALUES FOUND IN mman.h definitions
*/
#define PROT_READ        0x1                /* Page can be read.  */
#define PROT_WRITE        0x2                /* Page can be written.  */

# define MAP_ANONYMOUS        0x20                /* Don't use a file.  */

#define MAP_SHARED        0x01                /* Share changes.  */
#define MAP_PRIVATE        0x02                /* Changes are private.  */

int *buffer;

void producer() {
    for (int i=0; i<BUF_SIZE; i++) {
        printf("parent assigning address %p\n", &buffer[i]);
        buffer[i] = i;
    }
    return;
}

int consumer() {
    int sum = 0;
    for (int i=0; i<BUF_SIZE; i++) {
        printf("child reading address: %p\n", &buffer[i]);
        sum += buffer[i];
    }
        
    return(sum);
}

int main() {
    printf("it runs...\n");
    buffer = (int *) mmap(0, BUF_SIZE*sizeof(int), PROT_READ | PROT_WRITE, 
                          MAP_ANONYMOUS | MAP_SHARED, -1, 0);


    
    if (!buffer) {
        printf("Error: mmap() failed\n");
        exit(-1);
    }

    printf("it maps... mapped to: %p\n", buffer);
    for (int i = 0; i<BUF_SIZE;i++) {
        printf("assigning %d to position %p\n", i, &buffer[i]);
        buffer[i] = 0;
    }

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
    