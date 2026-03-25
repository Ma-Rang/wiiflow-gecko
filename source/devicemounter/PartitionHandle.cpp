 /****************************************************************************
 * Copyright (C) 2010 by Dimok
 *           (C) 2012 by FIX94
 *
 * Stripped for Dolphin Consolizer — FAT only, no WBFS/NTFS/EXT2.
 ***************************************************************************/
#include <gccore.h>
#include <stdio.h>
#include <string.h>
#include <sdcard/gcsd.h>
#include "PartitionHandle.h"
#include "fat.h"
#include "gui/text.hpp"
#include "loader/utils.h"
#include "memory/mem2.hpp"
#include "gecko/gecko.hpp"

#define PARTITION_TYPE_DOS33_EXTENDED		0x05
#define PARTITION_TYPE_WIN95_EXTENDED		0x0F

extern "C"
{
sec_t FindFirstValidPartition(const DISC_INTERFACE* disc);
}

#define CACHE 32
#define SECTORS 64

static inline const char *PartFromType(int type)
{
	switch (type)
	{
		case 0x01: return "FAT12";
		case 0x04: return "FAT16";
		case 0x06: return "FAT16";
		case 0x0b: return "FAT32";
		case 0x0c: return "FAT32";
		case 0x0e: return "FAT16";
		default: return "Unknown";
	}
}

void PartitionHandle::Init()
{
	interface = NULL;
}

void PartitionHandle::SetDevice(const DISC_INTERFACE *discio)
{
	Cleanup();
	interface = discio;

	if(!interface)
		{ gprintf("PH: no interface\n"); return; }

	if(!interface->startup())
		{ gprintf("PH: startup failed\n"); return; }

	if(!interface->isInserted())
		{ gprintf("PH: isInserted=false\n"); return; }

	gprintf("PH: card present, finding partitions\n");
	FindPartitions();
	gprintf("PH: found %d partition(s)\n", (int)PartitionList.size());
}

void PartitionHandle::Cleanup()
{
	UnMountAll();
	if(interface != NULL)
		interface->shutdown();
	interface = NULL;
	PartitionList.clear();
	MountNameList.clear();
}

bool PartitionHandle::IsMounted(int pos)
{
	if(pos < 0 || pos >= (int) MountNameList.size())
		return false;
	return MountNameList[pos].size() > 0;
}

bool PartitionHandle::Mount(int pos, const char *name, bool forceFAT)
{
	if(!name || strlen(name) > 8)
		return false;

	/* If no partitions were found but forceFAT is set, try mounting
	   the raw device (no MBR — common with Dolphin's virtual SD). */
	if(!valid(pos) && forceFAT && interface)
	{
		gprintf("PH: no partition table, trying raw FAT mount\n");
		if(pos >= (int)MountNameList.size())
			MountNameList.resize(pos + 1);
		MountNameList[pos] = name;
		if(fatMount(name, interface, 0, CACHE, SECTORS))
		{
			AddPartition("FAT", 0, 0xdeadbeaf, true, 0x0c, 0);
			gprintf("PH: raw FAT mount OK at %s:\n", name);
			return true;
		}
		gprintf("PH: raw FAT mount failed\n");
		MountNameList[pos].clear();
		return false;
	}

	if(!valid(pos))
		return false;

	UnMount(pos);

	if(pos >= (int)MountNameList.size())
		MountNameList.resize(pos + 1);

	MountNameList[pos] = name;

	char DeviceSyn[10];
	strcpy(DeviceSyn, name);
	strcat(DeviceSyn, ":");

	if(forceFAT && (strlen(GetFSName(pos)) == 0 || strcmp(GetFSName(pos), "Unknown") == 0))
	{
		if(fatMount(MountNameList[pos].c_str(), interface, 0, CACHE, SECTORS))
		{
			sec_t FAT_startSector = FindFirstValidPartition(interface);
			AddPartition("FAT", FAT_startSector, 0xdeadbeaf, true, 0x0c, 0);
			gprintf("FAT Partition at %s (forceFAT) mounted.\n", DeviceSyn);
			return true;
		}
	}

	if(strncmp(GetFSName(pos), "FAT", 3) == 0 || strcmp(GetFSName(pos), "GUID-Entry") == 0)
	{
		if(fatMount(MountNameList[pos].c_str(), interface, GetLBAStart(pos), CACHE, SECTORS))
		{
			gprintf("FAT Partition at %s mounted.\n", DeviceSyn);
			PartitionList[pos].FSName = "FAT";
			return true;
		}
	}

	MountNameList[pos].clear();
	return false;
}

void PartitionHandle::UnMount(int pos)
{
	if(!interface || (pos < 0 || pos >= (int)MountNameList.size()) || (MountNameList[pos].size() == 0))
		return;

	char DeviceSyn[10];
	strcpy(DeviceSyn, MountName(pos));
	strcat(DeviceSyn, ":");
	DeviceSyn[9] = '\0';

	const char *FSName = GetFSName(pos);
	if(strncmp(FSName, "FAT", 3) == 0)
	{
		fatUnmount(DeviceSyn);
		gprintf("FAT Partition at %s unmounted.\n", DeviceSyn);
	}

	MountNameList[pos].clear();
}

bool PartitionHandle::IsExisting(u64 lba)
{
	for(u32 i = 0; i < PartitionList.size(); ++i)
	{
		if(PartitionList[i].LBA_Start == lba)
			return true;
	}
	return false;
}

s8 PartitionHandle::FindPartitions()
{
	MASTER_BOOT_RECORD *mbr = (MASTER_BOOT_RECORD*)MEM2_alloc(MAX_BYTES_PER_SECTOR);
	if(mbr == NULL)
		return -1;

	if(!interface->readSectors(0, 1, mbr))
	{
		MEM2_free(mbr);
		return -1;
	}

	if(mbr->signature != MBR_SIGNATURE && mbr->signature != MBR_SIGNATURE_MOD)
	{
		MEM2_free(mbr);
		return -1;
	}

	for(u8 i = 0; i < 4; i++)
	{
		PARTITION_RECORD *partition = (PARTITION_RECORD *)&mbr->partitions[i];

		if(partition->type == PARTITION_TYPE_GPT)
		{
			s8 ret = CheckGPT();
			if(ret == 0)
				break;
		}
		if(partition->type == PARTITION_TYPE_DOS33_EXTENDED || partition->type == PARTITION_TYPE_WIN95_EXTENDED)
		{
			CheckEBR(i, le32(partition->lba_start));
			continue;
		}
		if(le32(partition->block_count) > 0 && !IsExisting(le32(partition->lba_start)))
		{
			AddPartition(PartFromType(partition->type), le32(partition->lba_start),
					le32(partition->block_count), (partition->status == PARTITION_BOOTABLE), partition->type, i);
		}
	}
	MEM2_free(mbr);
	return 0;
}

void PartitionHandle::CheckEBR(u8 PartNum, sec_t ebr_lba)
{
	EXTENDED_BOOT_RECORD *ebr = (EXTENDED_BOOT_RECORD*)MEM2_alloc(MAX_BYTES_PER_SECTOR);
	if(ebr == NULL)
		return;
	sec_t next_erb_lba = 0;

	do
	{
		if(!interface->readSectors(ebr_lba + next_erb_lba, 1, ebr))
		{
			MEM2_free(ebr);
			return;
		}
		if(ebr->signature != EBR_SIGNATURE && ebr->signature != EBR_SIGNATURE_MOD)
		{
			MEM2_free(ebr);
			return;
		}

		if(le32(ebr->partition.block_count) > 0 && !IsExisting(ebr_lba + next_erb_lba + le32(ebr->partition.lba_start)))
		{
			AddPartition(PartFromType(ebr->partition.type), ebr_lba + next_erb_lba + le32(ebr->partition.lba_start),
					le32(ebr->partition.block_count), (ebr->partition.status == PARTITION_BOOTABLE), ebr->partition.type, PartNum);
		}
		next_erb_lba = le32(ebr->next_ebr.lba_start);
	}
	while(next_erb_lba > 0);

	MEM2_free(ebr);
}

static const u8 TYPE_UNUSED[16] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
static const u8 TYPE_BIOS[16] = { 0x48,0x61,0x68,0x21,0x49,0x64,0x6F,0x6E,0x74,0x4E,0x65,0x65,0x64,0x45,0x46,0x49 };

s8 PartitionHandle::CheckGPT()
{
	GPT_HEADER *gpt_header = (GPT_HEADER*)MEM2_alloc(MAX_BYTES_PER_SECTOR);
	if(gpt_header == NULL)
		return -1;

	if(!interface->readSectors(1, 1, gpt_header))
	{
		MEM2_free(gpt_header);
		return -1;
	}
	if(strncmp(gpt_header->magic, "EFI PART", 8) != 0)
	{
		MEM2_free(gpt_header);
		return -1;
	}

	gpt_header->part_table_lba = le64(gpt_header->part_table_lba);
	gpt_header->part_entries = le32(gpt_header->part_entries);
	gpt_header->part_entry_size = le32(gpt_header->part_entry_size);
	gpt_header->part_entry_checksum = le32(gpt_header->part_entry_checksum);

	u8 *sector_buf = (u8*)MEM2_alloc(MAX_BYTES_PER_SECTOR);
	if(sector_buf == NULL)
	{
		MEM2_free(gpt_header);
		return -1;
	}
	u64 next_lba = gpt_header->part_table_lba;
	for(u32 i = 0; i < gpt_header->part_entries; ++i)
	{
		if(!interface->readSectors(next_lba, 1, sector_buf))
			break;

		for(u32 n = 0; n < BYTES_PER_SECTOR/gpt_header->part_entry_size; ++n, ++i)
		{
			GUID_PART_ENTRY *part_entry = (GUID_PART_ENTRY*)(sector_buf+gpt_header->part_entry_size*n);

			if(memcmp(part_entry->part_type_guid, TYPE_UNUSED, 16) == 0)
				continue;

			if(IsExisting(le64(part_entry->part_first_lba)))
				continue;

			bool bootable = (memcmp(part_entry->part_type_guid, TYPE_BIOS, 16) == 0);

			AddPartition("GUID-Entry", le64(part_entry->part_first_lba), le64(part_entry->part_last_lba)-le64(part_entry->part_first_lba), bootable, PARTITION_TYPE_GPT, i);
		}
		next_lba++;
	}
	MEM2_free(sector_buf);
	MEM2_free(gpt_header);
	return 0;
}

void PartitionHandle::AddPartition(const char *name, u64 lba_start, u64 sec_count, bool bootable, u8 part_type, u8 part_num)
{
	PartitionFS PartitionEntry;
	PartitionEntry.FSName = name;
	PartitionEntry.LBA_Start = lba_start;
	PartitionEntry.SecCount = sec_count;
	PartitionEntry.Bootable = bootable;
	PartitionEntry.PartitionType = part_type;
	PartitionEntry.PartitionNum = part_num;
	PartitionList.push_back(PartitionEntry);
}
