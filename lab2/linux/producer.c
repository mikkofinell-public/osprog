#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "common/togglecase.h"

#define error(x) do { printf("error: %s\n", x); exit(0); } while (0)

#define READ 0
#define WRITE 1

int main() {
    char * msg;
    int pfd_send[2], pfd_recv[2];
    long PIPE_BUF;
    ssize_t bytes;

    if (pipe(pfd_send) == -1) {
        error("creating pipe 1\n");
    }
    if (pipe(pfd_recv) == -1) {
        error("creating pipe 2\n");
    }

    /* guaranteed atomic pipe write size */
    PIPE_BUF = fpathconf(pfd_send[1], _PC_PIPE_BUF);

    switch (fork()) {
    case -1:
        error("fork");

    case 0:
        if (close(pfd_send[WRITE]) == -1) {
            error("child close parent write");
        }
        if (close(pfd_recv[READ]) == -1) {
            error("child close parent read");
        }

        msg = malloc(sizeof(char) * PIPE_BUF);
        memset(msg, '\0', sizeof(msg));

        bytes = read(pfd_send[READ], msg, PIPE_BUF);
        if (bytes == -1) {
            error("child read failed");
        }
        if (bytes == 0) {
            error("child read was 0 bytes");
        }

        /* process data */
        togglecase(msg, strlen(msg));

        bytes = write(pfd_recv[WRITE], msg, strlen(msg));
        if (bytes != strlen(msg)) {
            error("child write");
        }

        _exit(0);

    default:
        if (close(pfd_send[READ]) == -1) {
            error("parent close child read");
        }
        if (close(pfd_recv[WRITE]) == -1) {
            error("parent close write");
        }

        msg = malloc(sizeof(char) * PIPE_BUF);
        memset(msg, '\0', sizeof(msg));
        strcpy(msg, "Studying Operating Systems Is Fun!");

        bytes = write(pfd_send[WRITE], msg, strlen(msg));
        if (bytes != strlen(msg)) {
            error("parent write\n");
        }

        /* clear the buffer */
        memset(msg, '\0', sizeof(msg));

        bytes = read(pfd_recv[READ], msg, PIPE_BUF);
        if (bytes == -1) {
            error("parent read failed");
        }
        if (bytes == 0) {
            error("parent read was 0 bytes");
        }

        printf("parent: %s\n", msg);
    }

    return 0;
}
