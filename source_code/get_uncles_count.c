
#include "types.h"
#include "stat.h"
#include "user.h"

int main(void) {
    int pid, grandchild;
    for (int i = 0; i < 3; i++) {  // Create 3 child processes
        pid = fork();
        if (pid == 0) {  // Child process
            grandchild = fork();
            if (grandchild == 0 && i == 2) {  // Grandchild process
                printf(1,"PID:%d->", getpid());
                printf(1,"%d\n", get_uncle_count(getpid()));
                exit();
            }
            wait();  // Wait for the grandchild to finish
            exit();
        }
    }
    while (wait() != -1);  // Parent waits for all children to finish
    exit();
}