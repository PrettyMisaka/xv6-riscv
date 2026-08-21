//
// paravirtual block driver for tinyRiscvH.
//
// The hypervisor owns the real virtio device; the guest issues block
// I/O as a synchronous VS-mode hypercall (an ecall from the guest
// kernel, taken in HS-mode as scause=10).  b->data is guest kernel
// memory (identity-mapped), so its address is both the guest VA and
// the GPA the hypervisor translates to HPA before touching the disk.
//

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"

// hypercall numbers (must match core/src/trap.c in the hypervisor)
#define HVC_BLOCK_READ  0x1001
#define HVC_BLOCK_WRITE 0x1002

void
virtio_disk_init(void)
{
  // The real virtio device belongs to the hypervisor; nothing to set up.
}

void
virtio_disk_rw(struct buf *b, int write)
{
  uint64 ret;
  uint64 hc = write ? HVC_BLOCK_WRITE : HVC_BLOCK_READ;

  asm volatile(
    "mv a0, %[hc]\n"
    "mv a1, %[blk]\n"
    "mv a2, %[buf]\n"
    "ecall\n"
    "mv %[ret], a0\n"
    : [ret] "=r"(ret)
    : [hc] "r"(hc), [blk] "r"(b->blockno), [buf] "r"((uint64)b->data)
    : "a0", "a1", "a2", "memory");

  if(ret != 0)
    panic("virtio_disk_rw: hypervisor I/O failed");
}

void
virtio_disk_intr(void)
{
  // No device interrupts: the hypervisor does I/O synchronously.
}
