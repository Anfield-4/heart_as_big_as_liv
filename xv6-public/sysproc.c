#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int sys_settickets(void) {
  int tickets;
  if (argint(0, &tickets) < 0 || tickets < 1)
        return -1;
  acquire(&ptable.lock);
  myproc()->tickets = tickets; 
  release(&ptable.lock);
  return 0;
}

int sys_getpinfo(void) {
  struct pstat* pinfo;
  if(argptr(0, (void*)&pinfo, sizeof(*pinfo)) < 0)
  	return -1;
  	
  acquire(&ptable.lock);
  struct proc* p;
  for(p = ptable.proc; p != &(ptable.proc[NPROC]); ++p) {
    int index = p - ptable.proc;
    if(p->state != UNUSED) {
      pinfo->pid[index] = p->pid;
      pinfo->ticks[index] = p->ticks;
      pinfo->inuse[index] = p->inuse;
      pinfo->tickets[index] = p->tickets;
    }
  }
  release(&ptable.lock);
  return 0;
}

int sys_getsumticks(void){
	return sumticks;
}
int sys_resetticks(void){
	sumticks = 0;
	lastticks = 0;
	return 0;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
