#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

void main(uint64 pagetable, uint64 end);
void timerinit();

// entry.S needs one stack per CPU.
__attribute__ ((aligned (16))) char stack0[4096 * NCPU];

// a scratch area per CPU for machine-mode timer interrupts.
uint64 timer_scratch[NCPU][5];

// assembly code in kernelvec.S for machine-mode timer interrupt.
extern void timervec();

void start(uint64 pagetable, uint64 end)
{
    if(cpuid() == 0){
        uartinit();
        printfinit();
        printf("hello\n");
        printf("%p %p\n", (void*)pagetable, (void*)end);
    }

    // NOTE: Do NOT set vsie here — trapinithart() in main() must set vstvec FIRST
    // before enabling any interrupts. Setting vsie.STIE before vstvec is configured
    // causes VS-mode timer traps to jump to vstvec=0 and crash.
    // Interrupt enable is done by scheduler() -> intr_on() after trapinithart() runs.
    // w_sie(r_sie() | SIE_SEIE | SIE_STIE);
    // timerinit();

    // asm volatile("sret");
    main(pagetable, end);
    panic("main return");
}

// ask each hart to generate timer interrupts.
// VS-mode safe: stimecmp is S-mode accessible and aliases to vstimecmp
// (the hypervisor has already set henvcfg.STCE).  The M-mode CSRs
// (mie/menvcfg/mcounteren) are illegal in VS-mode and must not be touched.
// Must be called after trapinithart() has installed vstvec.
void
timerinit()
{
  // ask for the very first timer interrupt.
  w_stimecmp(r_time() + 1000000);
}