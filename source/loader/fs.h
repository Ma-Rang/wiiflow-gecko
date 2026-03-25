/* Stub — minimal reimplementation for Dolphin Consolizer build */
#ifndef _FS_H_
#define _FS_H_

#include <gctypes.h>

#ifdef __cplusplus
extern "C" {
#endif

u8 *ISFS_GetFile(const char *path, u32 *size, s32 identifier);

#ifdef __cplusplus
}
#endif

#endif /* _FS_H_ */
