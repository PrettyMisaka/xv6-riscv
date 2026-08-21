#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

void
plicinit(void)
{
  // Set the VIRTIO disk IRQ priority (used by the hypervisor).  The UART is
  // passed through to the guest and polled by the guest, so it is NOT enabled
  // here — otherwise the UART IRQ would fire at the hypervisor and loop.
  *(uint32*)(PLIC + VIRTIO0_IRQ*4) = 1;
}

void
plicinithart(void)
{
  int hart = cpuid();
  
  // enable the VIRTIO disk IRQ for this hart's S-mode.  The UART is polled by
  // the guest, so don't enable its IRQ (the hypervisor would otherwise receive
  // it endlessly since we don't claim it).
  *(uint32*)PLIC_SENABLE(hart) = (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(uint32*)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
int
plic_claim(void)
{
  int hart = cpuid();
  int irq = *(uint32*)PLIC_SCLAIM(hart);
  return irq;
}

// tell the PLIC we've served this IRQ.
void
plic_complete(int irq)
{
  int hart = cpuid();
  *(uint32*)PLIC_SCLAIM(hart) = irq;
}
