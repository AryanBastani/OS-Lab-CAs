#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[]) 
{
    if (argc != 3) 
    {
        printf(2, "Usage: copy_file source destination\n");
        exit();
    }

    if (copy_file(argv[1], argv[2]) < 0)
        printf(2, "Error: Copy failed\n");
    else
        printf(1, "Copy successful\n");

    exit();
}