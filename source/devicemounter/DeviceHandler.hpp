/****************************************************************************
 * Copyright (C) 2010 by Dimok
 *           (C) 2012 by FIX94
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 ***************************************************************************/
#ifndef DEVICE_HANDLER_HPP_
#define DEVICE_HANDLER_HPP_

#include "PartitionHandle.h"

enum
{
	PART_FS_FAT = 0,
	PART_FS_NTFS,
	PART_FS_EXT,
	PART_FS_WBFS,
};

enum
{
	SD = 0,
	USB1,
	USB2,
	USB3,
	USB4,
	USB5,
	USB6,
	USB7,
	USB8,
	MAXDEVICES
};

const char DeviceName[MAXDEVICES][8] =
{
	"sd",
	"usb1",
	"usb2",
	"usb3",
	"usb4",
	"usb5",
	"usb6",
	"usb7",
	"usb8",
};

class DeviceHandler
{
public:
	void Init();
	void MountAll();
	void UnMountAll();
	bool IsInserted(int dev);

	bool MountSD();
	bool SD_Inserted() { return sd.IsInserted(); }
	void UnMountSD() { sd.UnMountAll(); }

	int PathToDriveType(const char *path);
	const char *GetFSName(int dev);
	int GetFSType(int dev);
	const char *PathToFSName(const char *path) { return GetFSName(PathToDriveType(path)); }

	/* Stubs for removed WBFS/USB/Nand functionality */
	void OpenWBFS(int dev) { (void)dev; }
	bool PartitionUsableForNandEmu(int dev) { (void)dev; return false; }

private:
	PartitionHandle sd;
};

extern DeviceHandler DeviceHandle;

#endif
