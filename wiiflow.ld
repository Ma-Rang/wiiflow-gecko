/* Supplemental linker script for WiiFlow — provides symbols expected by newer libogc (2.10+) */
/* Using INSERT AFTER to add to existing sections rather than replacing them */

SECTIONS {
	.bss (NOLOAD) : {
		. = ALIGN(32);
		__gxregs = .;
		. += 0x100;

		. = ALIGN(8);
		__stack_end = .;
		. += 0x10000;
		__stack_addr = .;

		. = ALIGN(8);
		__intrstack_end = .;
		. += 0x4000;
		__intrstack_addr = .;
	} INSERT AFTER .bss;
}
