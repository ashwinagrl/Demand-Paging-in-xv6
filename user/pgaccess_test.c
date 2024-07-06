#include "kernel/param.h"
#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    char *arr_to_access; // Creating a heap array to experiment with the pgaccess function.

    // Using uint64 to store the accessed bits, this gives us a maxlimit of checking 32 pages only.
    uint64 accessed_bitmap;

    // Allocating 32 pages of memory to the array.
    arr_to_access = malloc(32 * PGSIZE);

    // Calling the pgaccess function for the array once, so that the bits are reset for once.
    pgaccess(arr_to_access, 32, &accessed_bitmap);

    // Accessing and modifying the page 11, 15 and 30 intentionally to check the accessed and dirty bits.
    arr_to_access[11 * PGSIZE] += 1; 
    arr_to_access[15 * PGSIZE] += 1;
    arr_to_access[30 * PGSIZE] += 1;


    pgaccess(arr_to_access, 32, &accessed_bitmap);
   

    for (int i = 0; i < 32; i++)
    {
        // checking for Dirty bit
        if (accessed_bitmap & ((uint64)1 << 2 * i))
        {
            printf("pgaccess: page %d is dirty  ", i);
        }
        else
        {
            printf("pgaccess: page %d is not dirty  ", i);
        }

        // checking for Accessed bit
        if (accessed_bitmap & ((uint64)1 << (2 * i + 1)))
        {
            printf("pgaccess: page %d is accessed  ", i);
        }
        else
        {
            printf("pgaccess: page %d is not accessed  ", i);
        }
        printf("\n");
    }

    // Freeing the allocated memory
    free(arr_to_access);    
    exit(0);
}

