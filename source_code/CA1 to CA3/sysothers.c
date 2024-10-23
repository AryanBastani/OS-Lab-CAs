#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int find_digital_root(int n)
{
    if(n % 9 == 0)
        return(9);
        
    return(n % 9);
} 

int sys_find_digital_root(void)
{
    return(find_digital_root(myproc()->tf->ebx));
}