        # qemu -kernel loads the kernel at 0x80000000
        # and causes each hart (i.e. CPU) to jump there.
        # kernel.ld causes the following code to
        # be placed at 0x80000000.
.section .text
.global _entry
_entry:
        # Direct UART write to test VS-mode UART access (UART0 = 0x10000000)
        li t0, 0x10000000
        li t1, 'V'
        sb t1, 0(t0)
        li t1, 'S'
        sb t1, 0(t0)
        li t1, 0x0a   # newline
        sb t1, 0(t0)

        # set up a stack for C.
        # stack0 is declared in start.c,
        # with a 4096-byte stack per CPU.
        # sp = stack0 + ((hartid + 1) * 4096)
        la sp, stack0
        li a4, 1024*4
        mv a5, tp
        addi a5, a5, 1
        mul a4, a4, a5
        add sp, sp, a4
        # jump to start() in start.c
        call start
spin:
        j spin
