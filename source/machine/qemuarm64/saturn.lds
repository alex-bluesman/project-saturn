ENTRY(start)
OUTPUT_ARCH(aarch64)
PHDRS
{
    text PT_LOAD ;
}
SECTIONS
{
    . = 0x40000000;

    _start = .;

    .text : {
        *(.text)
    } :text = 0x9090

    .data : {
        *(.data)
    } :text


    .bss : {
        *(.bss)
    } :text

    _end = . ;
}
