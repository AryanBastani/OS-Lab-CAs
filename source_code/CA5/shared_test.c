#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char* argv[]) {
    char* shared = open_sharedmem(1);
    shared[0] = 0;
    printf(1, "proc: %d --- value: %d\n", 0, shared[0]);

    for (int i = 1; i <= 5; i++) {
        if (fork() == 0) {
            char* shmem = open_sharedmem(1);
            shmem[0] += 1;
            printf(1, "proc: %d --- value: %d\n", i, shmem[0]);
            close_sharedmem(1);
            exit();
        }
    }
    for (int i = 0; i < 5; i++)
        wait();

    printf(1, "\nfinal value:\nproc: %d --- value: %d\n", 0, shared[0]);
    close_sharedmem(1);
    exit();
}