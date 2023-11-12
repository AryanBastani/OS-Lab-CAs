#include "types.h"
#include "user.h"
#include "fcntl.h"

void test_root(int n)
{
    int ebx_content, dig_root;

    asm volatile(
        "movl %%ebx, %0;"
        "movl %1, %%ebx;"
        : "=r"(ebx_content)
        : "r"(n)
    );

    dig_root = find_digital_root();

    asm volatile(
        "movl %0, %%ebx;"
        : : "r"(ebx_content)
    );

    printf(1, "%d\n", dig_root);
}

int main(int argc, char* argv[])
{
    test_root(atoi(argv[1]));
    exit();
}
