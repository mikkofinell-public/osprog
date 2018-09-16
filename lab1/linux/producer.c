#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

int main() {
    /* shared memory variables */
    const int SIZE = 4096;
    const char * name = "OS";
    const char * msg = "Studying Operating Systems Is Fun!";
    int shm_fd;
    void * ptr;

    /* semaphore variables */
    const char * sendsem_name = "sendsem";
    const char * recvsem_name = "recvsem";
    sem_t * sendsem;
    sem_t * recvsem;


    sendsem = sem_open(sendsem_name, O_CREAT, S_IRUSR|S_IWUSR, 0);
    recvsem = sem_open(recvsem_name, O_CREAT, S_IRUSR|S_IWUSR, 0);

    /* create shm + msg here
     *
     *
     */

    /* create the shared memory segment */
    shm_fd = shm_open(name, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    /* configure the size of the shared memory segment */
    ftruncate(shm_fd, SIZE);

    /* now map the shared memory segment in the address space of the process */
    ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        printf("Map failed\n");
        return -1;
    }
    /* Now write to the shared memory region. */
    sprintf(ptr, "%s", msg);

    /* inform consumer that data is ready for processing */
    sem_post(sendsem);
    sem_close(sendsem);

    printf("Producer: sent msg of size %lu\n", strlen(msg));

    /* wait for consumer to signal that data processing complete */
    sem_wait(recvsem);
    sem_close(recvsem);

    printf("Producer: reply received, unlink and exit\n");

    sem_unlink(sendsem_name);
    sem_unlink(recvsem_name);

    /* now read from the shared memory region */
    printf("Producer received: ");
    printf("%s\n", (char *)ptr);

    /* remove the shared memory segment */
    if (shm_unlink(name) == -1) {
        printf("Error removing %s\n",name);
        exit(-1);
    }

    return 0;
}
