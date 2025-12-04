#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

void
backtrace(void)
{
  struct proc *p = myproc();
  uint64 fp = r_fp();

  printf("backtrace:\n");

  while (fp) {
    if (PGROUNDDOWN(fp) != PGROUNDDOWN((uint64)p->kstack))
      break;

    uint64 ra = *(uint64*)(fp - 8);
    printf("  ra: 0x%p\n", ra);

    uint64 prev_fp = *(uint64*)(fp - 16);
    if (prev_fp == 0 || prev_fp == fp)
      break;

    fp = prev_fp;
  }
}