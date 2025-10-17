/**
 * Linker script for RISC-V bare-metal with newlib
 * 
 * IMPORTANT: Adjust MEMORY sections based on your actual hardware!
 * This is a generic template.
 */

OUTPUT_ARCH("riscv")
ENTRY(_start)

/* Memory layout - ADJUST THESE FOR YOUR HARDWARE */
MEMORY
{
    /* RAM: Adjust start address and length for your board */
    /* Example: 256MB RAM starting at 0x80000000 */
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 256M
}

/* Stack size - adjust as needed for OpenCV */
STACK_SIZE = 0x10000;  /* 64KB stack */

/* Heap size - OpenCV needs substantial heap for image processing */
HEAP_SIZE = 0x1000000;  /* 16MB heap - adjust based on your needs */

SECTIONS
{
    . = ORIGIN(RAM);
    
    /* Code section */
    .text : {
        KEEP(*(.text.init))
        KEEP(*(.text.start))
        *(.text*)
        *(.rodata*)
        . = ALIGN(8);
    } > RAM
    
    /* Global offset table (for position independent code) */
    .got : {
        *(.got*)
    } > RAM
    
    /* Initialized data */
    .data : {
        *(.data*)
        *(.sdata*)
        . = ALIGN(8);
    } > RAM
    
    /* Uninitialized data (BSS) */
    .bss : {
        __bss_start = .;
        *(.bss*)
        *(.sbss*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end = .;
    } > RAM
    
    /* End of used memory */
    _end = .;
    
    /* Heap section */
    .heap : {
        __heap_start = .;
        . = . + HEAP_SIZE;
        __heap_end = .;
        _heap_end = .;
    } > RAM
    
    /* Stack section (grows downward) */
    .stack : {
        . = . + STACK_SIZE;
        __stack_top = .;
    } > RAM
    
    /* Discard sections we don't need */
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}

/* Provide symbols for stack pointer initialization */
PROVIDE(__stack_size = STACK_SIZE);
PROVIDE(__heap_size = HEAP_SIZE);

