/* Stub — not used in Dolphin Consolizer build */
#ifndef _WBFS_H_
#define _WBFS_H_

#include <gctypes.h>

#define GAMES_DIR "%s:/wbfs"
#define DF_GC_GAMES_DIR "%s:/games"

extern char wii_games_dir[64];
extern char gc_games_dir[64];

/* WBFS disc stubs */
typedef void *wbfs_disc_t;
static inline wbfs_disc_t *WBFS_OpenDisc(u8 *id, char *path) { (void)id; (void)path; return (wbfs_disc_t*)0; }
static inline void WBFS_CloseDisc(wbfs_disc_t *disc) { (void)disc; }
static inline u32 wbfs_extract_file(wbfs_disc_t *disc, char *name, void **data) { (void)disc; (void)name; (void)data; return 0; }
static inline void WBFS_Close(void) {}

extern int currentPartition;

#endif /* _WBFS_H_ */
