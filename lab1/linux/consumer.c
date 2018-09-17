#include "common/togglecase.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

int main() {
    /* shared memory variables */
    const char * name = "OS";
    const int SIZE = 4096;
    int shm_fd;
    void * ptr;
    char * msg;
    int length = 0;

    /* semaphore variables */
    const char * sendsem_name = "sendsem";
    const char * recvsem_name = "recvsem";
    sem_t * sendsem;
    sem_t * recvsem;

    recvsem = sem_open(recvsem_name, O_CREAT, S_IRUSR|S_IWUSR, 0);
    sendsem = sem_open(sendsem_name, O_CREAT, S_IRUSR|S_IWUSR, 0);

    printf("Consumer: waiting for msg\n");

    /* wait for producer to signal data ready for processing */
    sem_wait(sendsem);
    sem_close(sendsem);

    /* open shm and process data here
     */

    /* open the shared memory segment */
    shm_fd = shm_open(name, O_RDWR, S_IRUSR|S_IWUSR);
    if (shm_fd == -1) {
        printf("shared memory failed\n");
        exit(-1);
    }

    /* now map the shared memory segment in the address space of the process */
    ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        printf("Map failed\n");
        exit(-1);
    }

    /* process msg here
     */

    printf("Consumer received msg of size %lu\n", strlen(ptr));
    msg = (char *)ptr;
    length = strlen(ptr);
    togglecase(msg, length);

    sem_post(recvsem);
    sem_close(recvsem);
    
    return 0;
}
