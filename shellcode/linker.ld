/* stolen from @xerub */
OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS {
    .text : {
        *(.text)
    }

    _etext = .;
    PROVIDE (etext = .);

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }
    _edata = .;
    PROVIDE (edata = .);

    __bss_start = .;
    __bss_start__ = .;

    .bss : {
        *(.bss)
        . = ALIGN(4);
    }
    . = ALIGN(4);

    _bss_end__ = . ; __bss_end__ = . ; __end__ = . ;

    _end = .;
    PROVIDE (end = .);
}
