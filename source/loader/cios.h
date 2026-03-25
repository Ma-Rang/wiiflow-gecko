/* Stub — not used in Dolphin Consolizer build */
#ifndef _CIOS_H_
#define _CIOS_H_

#include <gctypes.h>

#define IOS_TYPE_NORMAL_IOS 0

typedef struct {
	u32 Version;
	u32 Base;
} IOS_Info;

extern IOS_Info CurrentIOS;

static inline void IOS_GetCurrentIOSInfo(void) {}

#endif /* _CIOS_H_ */
