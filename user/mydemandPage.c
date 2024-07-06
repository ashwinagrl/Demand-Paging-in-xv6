#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define N 5000 // global array size - change to see effect. Try 3000, 5000, 10000
int glob[N];
int main()
{
    glob[0] = 2; // initialize with any integer value
    printf("global addr from user space: %x\n", glob);
    for (int i = 1; i < N; i++)
    {
        glob[i] = glob[i - 1];
        if (i % 1000 == 0)
            pgtPrint();
    }
    printf("Printing final page table:\n");
    pgtPrint();
    printf("Value: %d\n", glob[N - 1]);
    exit(0);
}
