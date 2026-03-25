/* Stub — minimal reimplementation of WiiFlow's ISFS_GetFile for Dolphin Consolizer build */
#include <string.h>
#include <gccore.h>
#include "memory/mem2.hpp"
#include "loader/fs.h"

u8 *ISFS_GetFile(const char *path, u32 *size, s32 identifier)
{
	(void)identifier;

	if(path == NULL)
		return NULL;

	s32 fd = ISFS_Open(path, ISFS_OPEN_READ);
	if(fd < 0)
		return NULL;

	fstats st ATTRIBUTE_ALIGN(32);
	memset(&st, 0, sizeof(fstats));

	if(ISFS_GetFileStats(fd, &st) < 0)
	{
		ISFS_Close(fd);
		return NULL;
	}

	u8 *buf = (u8 *)MEM2_alloc(st.file_length);
	if(buf == NULL)
	{
		ISFS_Close(fd);
		return NULL;
	}

	if(ISFS_Read(fd, buf, st.file_length) < 0)
	{
		MEM2_free(buf);
		ISFS_Close(fd);
		return NULL;
	}

	ISFS_Close(fd);
	if(size != NULL)
		*size = st.file_length;
	return buf;
}
