/*
 * NAND title bootstub — loaded at physical 0x3400 by rvl.ld.
 *
 * When a NAND title (channel) boots, Dolphin's HLE IOS sets PC = 0x3400
 * with address translation OFF (MSR.IR=0, MSR.DR=0).  This stub must:
 *   1. Enable extended BATs (HID4.SBE)
 *   2. Set up BAT registers for virtual -> physical mapping
 *      - MEM1: 0x80000000 -> 0x00000000 (cached)
 *      - MEM1: 0xC0000000 -> 0x00000000 (uncached)
 *      - MEM2: 0x90000000 -> 0x10000000 (cached)
 *      - MEM2: 0xD0000000 -> 0x10000000 (uncached)
 *   3. Enable address translation (MSR IR + DR)
 *   4. Jump to the DOL entry point (_start)
 */

#define r0 0
#define r3 3
#define r4 4

    .section .stub,"ax",@progbits
    .globl _stub_start

_stub_start:
    /* ---- Enable extended BATs (HID4.SBE = bit 1) ---- */
    /* HID4 = SPR 1011 */
    mfspr   r3, 1011
    oris    r3, r3, 0x4000      /* Set SBE (bit 1 from MSB = bit 30) */
    mtspr   1011, r3
    isync

    /* ---- Clear all BATs (0-3 standard, 4-7 extended) ---- */
    li      r0, 0
    mtspr   528, r0     /* IBAT0U */
    mtspr   530, r0     /* IBAT1U */
    mtspr   532, r0     /* IBAT2U */
    mtspr   534, r0     /* IBAT3U */
    mtspr   536, r0     /* DBAT0U */
    mtspr   538, r0     /* DBAT1U */
    mtspr   540, r0     /* DBAT2U */
    mtspr   542, r0     /* DBAT3U */
    /* Extended BATs (Wii-specific, SPR 560-575) */
    mtspr   560, r0     /* IBAT4U */
    mtspr   562, r0     /* IBAT5U */
    mtspr   564, r0     /* IBAT6U */
    mtspr   566, r0     /* IBAT7U */
    mtspr   568, r0     /* DBAT4U */
    mtspr   570, r0     /* DBAT5U */
    mtspr   572, r0     /* DBAT6U */
    mtspr   574, r0     /* DBAT7U */
    isync

    /* ---- MEM1 cached: 0x80000000 -> phys 0x00000000, 256MB ---- */
    /* IBAT0 + DBAT0 */
    lis     r3, 0x8000
    ori     r3, r3, 0x1FFF      /* BL = 0x1FFF = 256MB, Vs=1, Vp=1 */
    li      r4, 0x0002           /* BRPN = 0, PP = 0b10 (r/w) */

    mtspr   528, r3     /* IBAT0U */
    mtspr   529, r4     /* IBAT0L */
    mtspr   536, r3     /* DBAT0U */
    mtspr   537, r4     /* DBAT0L */

    /* ---- MEM1 uncached: 0xC0000000 -> phys 0x00000000, 256MB ---- */
    /* DBAT1 only (no IBAT needed for uncached access) */
    lis     r3, 0xC000
    ori     r3, r3, 0x1FFF
    li      r4, 0x002A           /* BRPN = 0, W=1, I=1, PP = 0b10 */

    mtspr   538, r3     /* DBAT1U */
    mtspr   539, r4     /* DBAT1L */

    /* ---- MEM2 cached: 0x90000000 -> phys 0x10000000, 64MB ---- */
    /* IBAT4 + DBAT4 */
    lis     r3, 0x9000
    ori     r3, r3, 0x07FF      /* BL = 0x07FF = 64MB, Vs=1, Vp=1 */
    lis     r4, 0x1000
    ori     r4, r4, 0x0002      /* BRPN = 0x10000000, PP = 0b10 */

    mtspr   560, r3     /* IBAT4U */
    mtspr   561, r4     /* IBAT4L */
    mtspr   568, r3     /* DBAT4U */
    mtspr   569, r4     /* DBAT4L */

    /* ---- MEM2 uncached: 0xD0000000 -> phys 0x10000000, 64MB ---- */
    /* DBAT5 only */
    lis     r3, 0xD000
    ori     r3, r3, 0x07FF
    lis     r4, 0x1000
    ori     r4, r4, 0x002A      /* BRPN = 0x10000000, W=1, I=1, PP = 0b10 */

    mtspr   570, r3     /* DBAT5U */
    mtspr   571, r4     /* DBAT5L */

    /* ---- Identity map: 0x00000000 -> phys 0x00000000, 256MB ---- */
    /* IBAT1: needed so rfi can fetch the next instruction while CPU */
    /* is still at physical 0x3400+ when translation turns on        */
    lis     r3, 0x0000
    ori     r3, r3, 0x1FFF
    li      r4, 0x0002

    mtspr   530, r3     /* IBAT1U */
    mtspr   531, r4     /* IBAT1L */

    isync

    /* ---- Enable address translation (MSR.IR | MSR.DR) ---- */
    mfmsr   r3
    ori     r3, r3, 0x30        /* IR (bit 26) + DR (bit 27) */
    mtsrr1  r3

    /* ---- Jump to _start (DOL entry point) ---- */
    lis     r3, _start@h
    ori     r3, r3, _start@l
    mtsrr0  r3

    rfi
