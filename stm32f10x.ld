/*
 * GCC linker script for STM32 microcontrollers (ARM Cortex-M).
 *
 * It exports the symbols needed for the CMSIS assembler startup script for GCC
 * ARM toolchains (_sidata, _sdata, _edata, _sbss, _ebss) and sets the entry
 * point to Reset_Handler.
 *
 * Adapt FLASH/RAM size for your particular device below.
 *
 * @author Bjørn Forsman
 */

MEMORY
{
    flash (rx)  : ORIGIN = 0x0800C000, LENGTH = 256K - 48K
    ram  (rwx)  : ORIGIN = 0x20000000, LENGTH = 48K
	CCMRAM (rw) : ORIGIN = 0x10000000, LENGTH = 64K
}

ENTRY(Reset_Handler)

/*
 * Reserve memory for heap and stack. The linker will issue an error if there
 * is not enough memory.
 *
 * NOTE: The reserved heap and stack will be added to the bss column of the
 * binutils size command.
 */
_heap_size = 0;    /* required amount of heap  */
_stack_size = 0;   /* required amount of stack */

/*
 * The stack starts at the end of RAM and grows downwards. Full-descending
 * stack; decrement first, then store.
 */
_estack = ORIGIN(ram) + LENGTH(ram);

SECTIONS
{
    /* Reset and ISR vectors */
    /* .isr_vector : */
    /* { */
    /*     __isr_vector_start__ = .; */
    /*     KEEP(*(.isr_vector))  *//* without 'KEEP' the garbage collector discards this section */
    /*     ASSERT(. != __isr_vector_start__, "The .isr_vector section is empty"); */
    /* } >flash */


    /* Text section (code and read-only data) */
    .text :
    {
        . = ALIGN(4);
        _stext = .;
		PROVIDE (smart_isr_vector_table_base = .);
        KEEP(*(.isr_vector .isr_vector.*))
        *(.text*)   /* code */
        *(.rodata*) /* read only data */

        /*
         * NOTE: .glue_7 and .glue_7t sections are not needed because Cortex-M
         * only supports Thumb instructions, no ARM/Thumb interworking.
         */

        /* Static constructors and destructors */
        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);
        _etext = .;
    } >flash

    /* 
	 * Data init section section
	 */
	.initcalldata.init :
	{
		. = ALIGN(4);
		__data_initcall_start = .;
		KEEP(*(.initcalldata.init))
		. = ALIGN(4);
		__data_initcall_end   = .;
	} >flash
	
	/* 
	 * Module init section section
	 */
	.initcallmodule.init :
	{
		. = ALIGN(4);
		__module_initcall_start = .;
		KEEP(*(.initcallmodule.init))
		. = ALIGN(4);
		__module_initcall_end   = .;
	} >flash
	
	/* 
	 * Task looper section section
	 */
	.loopermodule.looper :
	{
		. = ALIGN(4);
		__module_looper_start = .;
		KEEP(*(.loopermodule.looper))
		. = ALIGN(4);
		__module_looper_end   = .;
	} >flash


    /*
     * Stack unwinding and exception handling sections.
     *
     * ARM compilers emit object files with .ARM.extab and .ARM.exidx sections
     * when using C++ exceptions. Also, at least GCC emits those sections when
     * dividing large numbers (64-bit) in C. So we have to handle them.
     *
     * (ARM uses .ARM.extab and .ARM.exidx instead of the .eh_frame section
     * used on x86.)
     */
    .ARM.extab : /* exception unwinding information */
    {
        *(.ARM.extab*)
    } >flash
    .ARM.exidx : /* index entries for section unwinding */
    {
        *(.ARM.exidx*)
    } >flash


    /*
     * Newlib and Eglibc (at least) need these for C++ support.
     *
     * (Copied from Sourcery CodeBench Lite: arm-none-eabi-gcc -V)
     */
    .preinit_array :
    {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } >flash
    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
    } >flash
    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } >flash


    /*
     * Initialized data section. This section is programmed into FLASH (LMA
     * address) and copied to RAM (VMA address) in startup code.
     */
    _sidata = .;
    .data : AT(_sidata) /* LMA address is _sidata (in FLASH) */
    {
        . = ALIGN(4);
        _sdata = .; /* data section VMA address */
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >ram

	/* CCM-RAM section 
	* 
	* IMPORTANT NOTE! 
	* If initialized variables will be placed in this section,
	* the startup code needs to be modified to copy the init-values.  
	*/
	.ccmram :
	{
		. = ALIGN(4);
		_sccmram = .;       /* create a global symbol at ccmram start */
		*(.ccmram)
		*(.ccmram*)

		. = ALIGN(4);
		_eccmram = .;       /* create a global symbol at ccmram end */
	} >CCMRAM AT> flash
  
    /* Uninitialized data section (zeroed out by startup code) */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
		__bss_start__ = _sbss;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
		__bss_end__ = _ebss;
    } >ram

	
	/*
	* The heap consumes the remainder of the SRAM.
	* for malloc()
	* Add by Richile
	*/
	.heap (NOLOAD) :
	{
		. = ALIGN(4);
		_sheap = . ;
		/*
		 * This allows us to declare an object or objects up to the minimum acceptable
		 * heap size and receive a linker error if the space available for the heap is
		 * not sufficient.
		 */
		*(.heap)

		/* extend the heap up to the top of SRAM */
		. = ORIGIN(ram) + LENGTH(ram) - ABSOLUTE(_sheap);
		_eheap = .;
	} > ram


/* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
    
}

/* Nice to have */
/* __isr_vector_size__ = SIZEOF(.isr_vector); */
__text_size__ = SIZEOF(.text);
__data_size__ = SIZEOF(.data);
__bss_size__ = SIZEOF(.bss);
