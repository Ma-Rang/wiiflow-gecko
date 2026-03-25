/* Stub — GC disc parsing not used in Dolphin Consolizer build */
#ifndef _GCDISC_HPP_
#define _GCDISC_HPP_

#include <gctypes.h>

static inline bool DEVO_Installed(const char *dir) { (void)dir; return false; }
static inline bool Nintendont_Installed(void) { return false; }

class GC_Disc
{
public:
	void init(const char *path) { (void)path; }
	void clear() {}
	u8 *GetGameCubeBanner() { return NULL; }
};

extern GC_Disc GC_Disc_Reader;

#endif /* _GCDISC_HPP_ */
