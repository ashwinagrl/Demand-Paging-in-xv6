#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

// uint64, is a 64 bit number so there is a max limit of 32 pages, whose information can be stored because we are using 2 bits for each page.
#define MAXPAGES_TO_CHECK 32

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr; 
  int n;

  argint(0, &n);
  addr = myproc()->sz;

  // We do not need growproc while doing demand paging, as it allots the whole heap memory at once when the malloc is called by the user program.
  // By ensuring that the memory is not allocated here, we ensure that a page fault occurs when the user program tries to access the memory. 
  // Hence, we comment out the growproc function call.

  // if(growproc(n) < 0)
  //   return -1;

  // But still we need to increment the size of the process by n, as the program should get a virtual feel that the memory is allocated.
  myproc()->sz += n;

  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// System call to print the page table Entries which are valid and user accessible
uint64
sys_pgtPrint(){

  pagetable_t pagetable = myproc()->pagetable;

  uint64 va = 0;
  for(int i = 0; i < 512; i++){
    // pte_1 is the first level page table entry
    pte_t pte_1 = pagetable[i];

    if(pte_1 & PTE_V){ // Check if the page table entry is valid to go to the next level

      for(int j = 0; j < 512; j++){

        // pte_2 is the second level page table entry
        pte_t pte_2 = ((pte_t *)(PTE2PA(pte_1)))[j];

        if(pte_2 & PTE_V){

          for(int k = 0; k < 512; k++){
            // pte_3 is the third level page table entry
            pte_t pte_3 = ((pte_t *)(PTE2PA(pte_2)))[k];

            if((pte_3 & PTE_V)&&(pte_3 & PTE_U)){ // Check if the page table entry is valid and user accessible

              // i,j and k are the indexes of the page table entries in the level 1, level 2 and level 3 page tables respectively.
              // We combine i,j and k to get the virtual address of the page table entry, by shifting i by 30 bits, j by 21 bits and k by 12 bits.
              va = (uint64)((i << 30) | (j << 21) | (k << 12));

              printf("PTE No: %d, Virtual page address: %p, Physical page address: %p\n", k, va, PTE2PA(pte_3));
            }
          }
        }
      }
    }
  }
  return 0;
}

uint64
sys_pgaccess(void)
{
  uint64 starting_va; // It stores the starting virtual address of the first user page to check
  int pages_to_read;  // It stores the number of pages to check
  uint64 buffer_addr; // It stores the address of the buffer where the accessed bits are to be stored

  argaddr(0, &starting_va);
  argint(1, &pages_to_read);
  argaddr(2, &buffer_addr);


  uint64 bitmap_value = 0;  // It stores the accessed and dirty bits at odd and even indices respectively. 


  struct proc *p = myproc();
  
  if(pages_to_read > MAXPAGES_TO_CHECK){
    pages_to_read = MAXPAGES_TO_CHECK;
  }

  for (int i = 0; i < pages_to_read; ++i) {

    pte_t *pte = walk(p->pagetable, starting_va + i*PGSIZE, 0);

    // Checking for Dirty page
    if(*pte & PTE_D){
      bitmap_value = ((bitmap_value) | ((uint64)1 << (2*i)));
    }

    // Checking for Accessed page
    if(*pte & PTE_A){
      bitmap_value = ((bitmap_value) | ((uint64)1 << (2*i + 1)));
    }

    // Resetting the accessed bit as we have walked the page table entry
    *pte = (*pte & ~PTE_A);
  }
  
  // Copying the bitmap value to the user buffer
  copyout(p->pagetable, buffer_addr, (char *)&bitmap_value, sizeof(bitmap_value));

  return 0;
}