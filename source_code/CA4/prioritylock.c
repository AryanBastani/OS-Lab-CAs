#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "prioritylock.h"

void init_pr(struct prioritylock* lk, char* name) 
{
    initlock(&lk->lk, "prioritylock");
    lk->name = name;
    lk->locked = 0;
    lk->pid = 0;
}

int priority(struct prioritylock* lk) 
{
    if (lk->num_of_procs == 0 || lk->processes[0] != myproc()->pid)
        return 0;

    for (int i = 0; i < lk->num_of_procs - 1; i++)
        lk->processes[i] = lk->processes[i + 1];
    lk->processes[lk->num_of_procs - 1] = 0;
    lk->num_of_procs--;
    return 1;
}

int acquire_pr(struct prioritylock* lk) 
{

    int is_holding;
    acquire(&lk->lk);
    is_holding = lk->locked && (lk->pid == myproc()->pid);
    if(is_holding)
    {
        release(&lk->lk);
        return(-1);
    }

    if (lk->num_of_procs < NPROC)
    {
        int new_proc_indx = 0;
        while (lk->processes[new_proc_indx] > myproc()->pid)
            new_proc_indx++;
        for (int i = lk->num_of_procs; i > new_proc_indx; i--)
            lk->processes[i] = lk->processes[i - 1];
        lk->processes[new_proc_indx] = myproc()->pid;
        lk->num_of_procs++;
    }

    while (lk->locked == 1 || !priority(lk)) 
    {
        release(&lk->lk);
        acquire(&lk->lk);
    }

    cprintf("This is the priority list:\n");
    for (int i = 0; i < lk->num_of_procs; i++) 
    {
        cprintf("%d", lk->processes[i]);
        if (i != lk->num_of_procs - 1)
            cprintf(", ");
        else
            cprintf("\n");
    }

    cprintf("process %d is acquiring the lock\n", myproc()->pid);

    lk->locked = 1;
    lk->pid = myproc()->pid;
    release(&lk->lk);

    return(lk->pid);
}

int release_pr(struct prioritylock* lk) 
{

    int is_holding;
    acquire(&lk->lk);
    is_holding = lk->locked && (lk->pid == myproc()->pid);
    if(!is_holding)
    {
        release(&lk->lk);
        return(-1);
    }

    cprintf("process %d is releasing the lock\n", lk->pid);

    lk->locked = 0;
    lk->pid = 0;
    release(&lk->lk);

    return(0);
}
