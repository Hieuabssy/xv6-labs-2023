#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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
  if(growproc(n) < 0)
    return -1;
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


#ifdef LAB_PGTBL
pte_t *walk(pagetable_t pagetable, uint64 va, int alloc);
int
sys_pgaccess(void)
{
  uint64 base;
  int len;
  uint64 user_mask;

  // --- SỬA LẠI ĐOẠN NÀY ---
  // Gọi hàm trực tiếp, không đặt trong if
  argaddr(0, &base);
  argint(1, &len);
  argaddr(2, &user_mask);
  // -----------------------

  // Giới hạn len (max 64 trang)
  if(len > 64 || len < 0) 
    return -1;

  struct proc *p = myproc();
  uint64 bitmask = 0; 

  // ... (Phần code xử lý logic bên dưới giữ nguyên) ...
  
  // Nhắc lại logic xử lý bên dưới để bạn tiện copy:
  for(int i = 0; i < len; i++){
    uint64 va = base + i * PGSIZE;
    pte_t *pte = walk(p->pagetable, va, 0);

    if(pte != 0 && (*pte & PTE_V) && (*pte & PTE_U)){
      if(*pte & PTE_A){
        bitmask |= (1L << i);
        *pte &= ~PTE_A; // Xóa bit A
      }
    }
  }

  if(copyout(p->pagetable, user_mask, (char *)&bitmask, sizeof(bitmask)) < 0)
    return -1;

  return 0;
}
#endif

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
