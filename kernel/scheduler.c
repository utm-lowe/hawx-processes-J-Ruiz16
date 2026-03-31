#include "types.h"
#include "proc.h"
#include "scheduler.h"
#include "riscv.h"
//Copilot and ChatGpt help me with the file. 
// Extern declarations
extern struct proc proc[];
extern struct cpu cpu;
extern void swtch(struct context *old, struct context *new);

// Simple round-robin scheduler based on xv6
// Iterates through the proc table looking for a RUNNABLE process,
// switches to it, and when it yields back, continues to the next process.
void
scheduler(void)
{
  struct proc *p;

  // Loop forever
  while(1) {
    intr_on();
    // Iterate through process table looking for process to run.
    for(p = proc; p < &proc[NPROC]; p++) {
      if(p->state == RUNNABLE) {
        // Switch to chosen process. It is the process's job
        // to release the lock and then reacquire it
        // before jumping back to us.
        p->state = RUNNING;
        cpu.proc = p;

        // Switch to the process's kernel context and run it
        swtch(&cpu.context, &p->context);

        // Process has ceded control. It is no longer running on this cpu.
        cpu.proc = 0;
        break;
      }
    }
  }
}

// Yield the CPU. Signal that the process is willing to surrender the CPU
// This is typically called from a timer interrupt or when the process
// voluntarily yields.
void
yield(void)
{
  struct proc *p = cpu.proc;
  
  if(p != 0) {
    // Change process state back to runnable
    p->state = RUNNABLE;

    // Switch back to scheduler context
    swtch(&p->context, &cpu.context);
  }
}
