/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */
 
 /*------------------------------------------------------------------------------
 *      Linker script for running in external SDRAM on the AT91SAM9G20
 *----------------------------------------------------------------------------*/

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(entry)

MEMORY
{
    sram0   (W!RX) : ORIGIN = 0x200000, LENGTH = 0x4000
    sram1   (W!RX) : ORIGIN = 0x300000, LENGTH = 0x4000
    sdram   (W!RX) : ORIGIN = 0x20000000, LENGTH = 0x04000000
}

SECTIONS
{  
    .fixed :
    {
        . = ALIGN(4);
        _sfixed = .;
        KEEP(*(.startup));
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        *(.rodata.*)
        *(.glue_7)
        *(.glue_7t)
        *(.vfp11_veneer)
        *(.v4_bx)
        *(.data)
        *(.data.*)
        *(.dyn)
        *(.iplt)
        *(.igot)
        *(.igot.plt)
        *(.rel.dyn)
        *(.rel.iplt)
        . = ALIGN(4);
        _efixed = .;
    } >sdram
    
   .ARM.exidx : AT (_efixed)
   {
        * (.ARM.exidx)
        _eexidx = .;
   } >sdram
   
   .ARM.extab : AT (_eexidx)
   {
        * (.ARM.extab)
        _eextab = .;
   } >sdram

    .prerelocate : AT (_eextab)
    {
        . = ALIGN(4);
        _sprerelocate = .;
        . = ALIGN(4);
        _eprerelocate = .;
    } >sram0

    .postrelocate : AT (_eextab + SIZEOF(.prerelocate))
    {
        . = ALIGN(4);
        _spostrelocate = .;
        KEEP(*(.vectors));
        *(.sramfunc)
        *(.ramfunc)
        . = ALIGN(4);
        _epostrelocate = .;
    } >sram0
    
    .sramdata : AT (_eextab + SIZEOF(.prerelocate) + SIZEOF(.postrelocate))
    {
        . = ALIGN(4);
        _ssramdata = .;
        *(.sramdata)
        . = ALIGN(4);
        _esramdata = .;
    } >sram1
    
    .srambss (NOLOAD) : {
        . = ALIGN(4);
        _ssrambss = .;
        *(.srambss)
        . = ALIGN(4);
        _esrambss = .;
    } >sram1
    
    .bss (NOLOAD) : {
        _szero = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        *(COMMON.*)
        . = ALIGN(4);
        _ezero = .;
    } >sdram
    
    _sheap_ = .;
    _eheap_ = 0x23F00000;
    
    /* Stack starts 1024 bytes before the end of SDRAM. This 1kB of space can be used for special purposes. */
    _sstack = 0x24000000 - 1024;
}
end = .;

