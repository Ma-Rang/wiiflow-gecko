/****************************************************************************
 * Copyright (C) 2012 FIX94
 * Modified for Dolphin Consolizer — game list from Consolizer via USB Gecko.
 ****************************************************************************/
#include <string.h>
#include <dirent.h>
#include <algorithm>
#include "ListGenerator.hpp"
#include "devicemounter/DeviceHandler.hpp"
#include "consolizer/consolizer.hpp"
#include "gui/coverflow.hpp"
#include "gui/text.hpp"
#include "loader/sys.h"

ListGenerator m_cacheList;
dir_discHdr ListElement;
Config CustomTitles;
GameTDB gameTDB;

void ListGenerator::Init(const char *settingsDir, const char *Language)
{
	if(settingsDir != NULL)
	{
		gameTDB_Path = fmt("%s/wiitdb.xml", settingsDir);
		CustomTitlesPath = fmt("%s/" CTITLES_FILENAME, settingsDir);
	}
	if(Language != NULL) gameTDB_Language = Language;
}

void ListGenerator::Clear(void)
{
	m_cacheList.clear();
	vector<dir_discHdr>().swap(m_cacheList);
}

void ListGenerator::OpenConfigs()
{
	gameTDB.OpenFile(gameTDB_Path.c_str());
	if(gameTDB.IsLoaded())
		gameTDB.SetLanguageCode(gameTDB_Language.c_str());
	CustomTitles.load(CustomTitlesPath.c_str());
	CustomTitles.groupCustomTitles();
}

void ListGenerator::CloseConfigs()
{
	if(gameTDB.IsLoaded())
		gameTDB.CloseFile();
	if(CustomTitles.loaded())
		CustomTitles.unload();
}

void ListGenerator::CreateListFromConsolizer()
{
	Clear();
	OpenConfigs();

	/* Get game list from Consolizer via USB Gecko */
	ConsolizerGameEntry entries[512];
	int count = Consolizer_RequestGameList(entries, 512);

	for(int i = 0; i < count; i++)
	{
		memset((void*)&ListElement, 0, sizeof(dir_discHdr));
		ListElement.index = m_cacheList.size();

		strncpy(ListElement.id, entries[i].game_id, 6);
		ListElement.settings[0] = entries[i].boot_token; /* CRC32 of path — sent back with LAUNCH */

		/* Look up title and metadata from GameTDB */
		ListElement.casecolor = 0xFFFFFF;
		char CustomTitle[64];
		memset(CustomTitle, 0, sizeof(CustomTitle));
		strncpy(CustomTitle, CustomTitles.getString("TITLES", ListElement.id).c_str(), 63);

		const char *gameTDB_Title = NULL;
		if(gameTDB.IsLoaded())
		{
			int PublishDate = gameTDB.GetPublishDate(ListElement.id);
			ListElement.year = PublishDate >> 16;
			ListElement.casecolor = gameTDB.GetCaseColor(ListElement.id);
			ListElement.wifi = gameTDB.GetWifiPlayers(ListElement.id);
			ListElement.players = gameTDB.GetPlayers(ListElement.id);
			if(strlen(CustomTitle) == 0)
				gameTDB.GetTitle(ListElement.id, gameTDB_Title);
		}
		if(!ValidColor(ListElement.casecolor))
			ListElement.casecolor = CoverFlow.InternalCoverColor(ListElement.id, 0xFFFFFF);

		if(strlen(CustomTitle) > 0)
			mbstowcs(ListElement.title, CustomTitle, 63);
		else if(gameTDB_Title != NULL && gameTDB_Title[0] != '\0')
			mbstowcs(ListElement.title, gameTDB_Title, 63);
		else if(entries[i].title[0] != '\0')
			mbstowcs(ListElement.title, entries[i].title, 63);
		else
			mbstowcs(ListElement.title, entries[i].game_id, 63);
		Asciify(ListElement.title);

		if(entries[i].type == CONSOLIZER_TYPE_GC)
			ListElement.type = TYPE_GC_GAME;
		else
			ListElement.type = TYPE_WII_GAME;
		m_cacheList.push_back(ListElement);
	}

	CloseConfigs();
}

/* --- File enumeration (originally in this file, needed by video.cpp, MusicPlayer, etc.) --- */

static inline bool IsFileSupported(const char *File, const vector<string>& FileTypes)
{
	auto fileName = std::string(File);
	for(auto & fileType : FileTypes)
	{
		if(fileName.length() >= fileType.length() &&
		    std::equal(fileName.end() - fileType.length(),
		               fileName.end(), fileType.begin(),
		               [](const char & c1, const char & c2)
		               { return (c1 == c2 || std::toupper(c1) == std::toupper(c2)); }))
			return true;
	}
	return false;
}

static char *FullPathChar = NULL;
static dirent *pent = NULL;
static DIR *pdir = NULL;

void GetFiles(const char *Path, const vector<string>& FileTypes,
				FileAdder AddFile, bool CompareFolders, u32 max_depth, u32 depth)
{
	vector<string> SubPaths;
	pdir = opendir(Path);
	if(pdir == NULL)
		return;
	while((pent = readdir(pdir)) != NULL)
	{
		if(pent->d_name[0] == '.')
			continue;
		FullPathChar = fmt("%s/%s", Path, pent->d_name);
		if(pent->d_type == DT_DIR)
		{
			if(CompareFolders && IsFileSupported(pent->d_name, FileTypes))
			{
				AddFile(FullPathChar);
				continue;
			}
			else if(depth < max_depth && strcmp(pent->d_name, "samples") != 0)
				SubPaths.push_back(FullPathChar);
		}
		else if(pent->d_type == DT_REG)
		{
			if(IsFileSupported(pent->d_name, FileTypes))
			{
				AddFile(FullPathChar);
				continue;
			}
		}
	}
	closedir(pdir);
	for(vector<string>::const_iterator p = SubPaths.begin(); p != SubPaths.end(); ++p)
		GetFiles(p->c_str(), FileTypes, AddFile, CompareFolders, max_depth, depth + 1);
	SubPaths.clear();
}
