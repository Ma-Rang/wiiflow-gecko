/****************************************************************************
 * Copyright (C) 2010 by Dimok
 *           (C) 2012 by FIX94
 *
 * Stripped for Dolphin Consolizer — SD card only, stock IOS 58.
 ***************************************************************************/
#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <ogc/system.h>
#include <sdcard/gcsd.h>
#include "DeviceHandler.hpp"
#include "fat.h"
#include "wiisd_libogc.h"

DeviceHandler DeviceHandle;

void DeviceHandler::Init()
{
	sd.Init();
}

void DeviceHandler::MountAll()
{
	MountSD();
}

bool DeviceHandler::MountSD()
{
	if(!sd.IsInserted() || !sd.IsMounted(0))
	{
		sd.SetDevice(&__io_wiisd_ogc);
		return sd.Mount(0, DeviceName[SD], true); /* Force FAT */
	}
	return true;
}

bool DeviceHandler::IsInserted(int dev)
{
	if(dev == SD)
		return sd.IsInserted() && sd.IsMounted(0);
	return false;
}

void DeviceHandler::UnMountAll()
{
	sd.UnMountAll();
	sd.Cleanup();
}

int DeviceHandler::PathToDriveType(const char *path)
{
	if(!path)
		return -1;
	if(strncasecmp(path, DeviceName[SD], strlen(DeviceName[SD])) == 0)
		return SD;
	return -1;
}

const char *DeviceHandler::GetFSName(int dev)
{
	if(dev == SD)
		return sd.GetFSName(0);
	return "";
}

int DeviceHandler::GetFSType(int dev)
{
	const char *FSName = GetFSName(dev);
	if(!FSName) return -1;
	if(strncmp(FSName, "FAT", 3) == 0)
		return PART_FS_FAT;
	return -1;
}
