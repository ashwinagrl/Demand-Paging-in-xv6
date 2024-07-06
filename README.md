# xv6 Paging Project

This project is a course assignment focused on implementing and understanding paging in the xv6 operating system. It involves modifying the xv6 kernel to handle system calls, implement demand paging, and detect accessed/dirty pages.

## Overview

The project consists of three main tasks:
1. Printing the page table entries.
2. Implementing demand paging.
3. Detecting accessed and/or dirty pages.

## Highlights

### Task 1: Printing the Page Table Entries
- **Objective:** Create a new system call `pgtPrint()` to print the page table entries for the current process.
- **Steps:**
  - Define the system call number in `syscall.h`.
  - Add a pointer to the system call in `syscall.c`.
  - Implement the system call function in `sysproc.c`.
  - Print page table entries in a specific format.

### Task 2: Implement Demand Paging
- **Objective:** Implement demand paging to allocate memory only when it is needed.
- **Steps:**
  - Modify `exec()` to prevent memory allocation for global variables during process creation.
  - Handle page faults in `trap.c` to allocate and map pages on demand.

### Task 3: Detect Accessed/Dirty Pages
- **Objective:** Implement logic to detect which pages have been accessed or modified.
- **Steps:**
  - Pass arguments to the system call using `argaddr()` and `argint()`.
  - Create a bitmask to store the accessed and dirty bits.
  - Walk through the page table to check and update the bitmask.
  - Copy the bitmask to the user address using `copyout()`.

## What I Learned

1. **Kernel Modifications:** Gained experience in modifying the xv6 kernel to add new system calls and manage page tables.
2. **Demand Paging:** Learned how to implement demand paging by handling page faults and managing memory allocation dynamically.
3. **Page Table Management:** Understood how to traverse and manipulate multi-level page tables to extract and print page table entries.
4. **Accessed/Dirty Bit Detection:** Developed logic to detect accessed and dirty pages and interpret the results using a bitmask.

## Experiment Observations

- **Global Array:** Declaring a large global array increases the number of valid page table entries.
- **Local Array:** Accessing a large local array can cause page faults due to limited stack space.
- **Heap Array:** Allocating a large array on the heap increases the number of valid page table entries as pages are allocated on demand.

## System Call Code Example

```c
uint64 sys_pgtPrint(void) {
  struct proc *p = myproc();
  for (int i = 0; i < 512; i++) {
    pte_t *pte = walk(p->pagetable, i * PGSIZE, 0);
    if (pte && (*pte & PTE_V) && (*pte & PTE_U)) {
      printf(1, "PTE No: %d, Virtual page address: 0x%lx, Physical page address: 0x%lx\n", i, i * PGSIZE, PTE2PA(*pte));
    }
  }
  return 0;
}
```
## Conclusion

This project provided valuable insights into the workings of system calls, paging, and memory management in the xv6 operating system. By implementing demand paging and page access detection, I enhanced my understanding of critical operating system concepts and gained practical kernel programming skills. The project emphasized the importance of efficient memory management and the ability to handle dynamic memory allocation through demand paging. Overall, this assignment was a significant step in deepening my knowledge and experience in operating system development.
