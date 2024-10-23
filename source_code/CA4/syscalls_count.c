#include "fcntl.h"
#include "types.h"
#include "user.h"


int main() 
{
    unlink("temp.txt");
    int fd = open("temp.txt", O_CREATE | O_WRONLY);

    for (int i = 0; i < 12; i++) 
    {
        int pid = fork();
        if (pid == 0) 
        {
            write(fd, "$hednejradam$_$eiataila$\n", 24);
            
            exit();
        }
    }
    while (1)
    {
        if(wait() == -1)
            break;
    }

    write(fd, "\n", 1);
    close(fd);

    print_syscalls();
    
    exit();
}