ENTRY(boot_entry)

PHDRS
{
    text PT_LOAD FLAGS(5);    /* 5 = Read + Execute */
    data PT_LOAD FLAGS(6);    /* 6 = Read + Write */
}

SECTIONS
{
    . = 1M;

    /* Executable code segment */
    .multiboot ALIGN(4) : 
    {
        *(.multiboot)
    } : text

    .text ALIGN(4) : 
    {
        *(.text)
    } : text

    /* Data segment */
    .data ALIGN(4) : 
    {
        *(.data)
    } : data

    .bss ALIGN(4) : 
    {
        *(.bss)
        *(COMMON)
    } : data
}
