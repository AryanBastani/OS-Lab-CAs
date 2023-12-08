#include "types.h"
#include "user.h"
#include "stat.h"

int main(void) {
    int pid = fork();

    if (pid < 0) {
        printf(1, "Fork failed\n");
        exit();
    }

    if (pid == 0) {
        // Child process
        printf(1, "Child process created\n");
        sleep(100); // Sleep for 10 seconds
        int child_lifetime = get_process_lifetime(getpid());
        printf(1, "Child process lifetime: %d ticks\n", child_lifetime);
        exit();
    } else {
        // Parent process
        wait(); // Wait for the child to finish
        int parent_lifetime = get_process_lifetime(getpid());
        printf(1, "Parent process lifetime: %d ticks\n", parent_lifetime);
        printf(1, "1 tick = 0.1 seconds\n");
        exit();
    }
}