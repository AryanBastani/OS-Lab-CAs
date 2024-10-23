#include "fcntl.h"
#include "types.h"
#include "user.h"

#define LOG_NAME "pr_log"
#define MSG_MAX 100
#define NUM_MAX 10
#define MSG_FIRST_PART "Process with pid = "    
#define SLEEP_MSG "acquired the lock and is gonna sleep"
#define RELEASED "released the prioritylock"


int int_max(int number)
{
    int size = 1;
    int helper = 10;
    while(1)
    {
        if(number < helper)
            return((helper / 10));
        size++;
        helper *= 10;
    }
}

void int_to_str(int number, char* string)
{
    int i = 0;
    if(number < 0)
    {
        number = -number;
        string[i] = '-';
        i++;
    }
    int max = int_max(number);
    max *= 10;
    do
    {
        number %= max;
        max /= 10;
        int a = number / max;
        string[i] = '0' + a;
        i++;
    } while (max != 1); 
}

void short_to_long(int begin, char* short_str, char* long_str, int len)
{
    for(int i = 0; i < len; i++)
        long_str[i + begin] = short_str[i];
}

void gen_massg(int pid, char* new_massg, char* text)
{

    short_to_long(0, MSG_FIRST_PART, new_massg, strlen(MSG_FIRST_PART));

    char pid_str[NUM_MAX]  = {0};
    memset(pid_str, 0, NUM_MAX);
    int_to_str(pid, pid_str);
    short_to_long(strlen(new_massg), pid_str, new_massg, strlen(pid_str));

    new_massg[strlen(new_massg)] = ' ';

    short_to_long(strlen(new_massg), text, new_massg, strlen(text));

    new_massg[strlen(new_massg)] = '\n';
}

int main() 
{
    unlink(LOG_NAME);
    int fd = open(LOG_NAME, O_CREATE | O_WRONLY);
    char massg[MSG_MAX] = {0};
    
    for (int i = 0; i < 12; i++) 
    {
        int pid = fork();

        if (pid == 0) 
        {
            pid = acquire_prior();

            memset(massg, 0, MSG_MAX);
            gen_massg(pid, massg, SLEEP_MSG);
            write(fd, massg, strlen(massg));
            sleep(100);

            release_prior();   
            memset(massg, 0, MSG_MAX);
            gen_massg(pid, massg, RELEASED);
            write(fd, massg, strlen(massg));    
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
    exit();
}