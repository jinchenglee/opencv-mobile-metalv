/**
 * Minimal startup code for RISC-V bare-metal
 * Initializes stack, clears BSS, and calls main()
 */

    .section .text.init
    .global _start
    .type _start, @function

_start:
    /* Disable interrupts */
    csrw mie, zero
    
    /* Set up stack pointer */
    la sp, __stack_top
    
    /* Clear BSS section */
    la t0, __bss_start
    la t1, __bss_end
clear_bss:
    bge t0, t1, clear_bss_done
    sd zero, 0(t0)
    addi t0, t0, 8
    j clear_bss
clear_bss_done:

    /* Call global constructors (for C++) */
    call __libc_init_array
    
    /* Call main with argc=0, argv=NULL */
    li a0, 0
    li a1, 0
    call main
    
    /* If main returns, call exit */
    call exit
    
    /* Should never reach here, but just in case */
hang:
    wfi
    j hang

    .size _start, . - _start

