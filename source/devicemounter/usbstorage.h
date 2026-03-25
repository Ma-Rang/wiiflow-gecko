/* Stub — USB storage not used in Dolphin Consolizer build */
#ifndef _USBSTORAGE2_H_
#define _USBSTORAGE2_H_

#include <gctypes.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline s32 USBStorage2_Init(u32 port) { (void)port; return -1; }
static inline void USBStorage2_Deinit(void) {}
static inline s32 USBStorage2_GetCapacity(u32 port, u32 *size) { (void)port; (void)size; return -1; }

#ifdef __cplusplus
}
#endif

#endif /* _USBSTORAGE2_H_ */
