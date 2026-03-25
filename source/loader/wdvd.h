/* Stub — not used in Dolphin Consolizer build */
#ifndef _WDVD_H_
#define _WDVD_H_

#include <gctypes.h>

static inline void WDVD_Init(void) {}
static inline void WDVD_Close(void) {}
static inline s32 WDVD_GetCoverStatus(u32 *status) { if(status) *status = 0; return 0; }

#endif /* _WDVD_H_ */
