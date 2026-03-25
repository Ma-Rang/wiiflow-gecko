/****************************************************************************
 * Copyright (C) 2012 FIX94
 * Modified for Dolphin Consolizer — game list comes from Consolizer via USB Gecko.
 ****************************************************************************/
#ifndef _LISTGENERATOR_HPP_
#define _LISTGENERATOR_HPP_

#include <string>
#include <vector>
#include <stdio.h>

#include "defines.h"
#include "types.h"
#include "config/config.hpp"
#include "loader/disc.h"
#include "gui/GameTDB.hpp"

using std::string;
using std::vector;

#define CONFIG_FILENAME_SKIP_DOMAIN		"PLUGINS"
#define CONFIG_FILENAME_SKIP_KEY		"filename_skip_regex"
#define CONFIG_FILENAME_SKIP_DEFAULT	"((dis[ck]|tape|side|track)[ _-]([b-l][^a-z]|0*[2-9]|0*[1-9][0-9]))|(^disc2[.]iso$)|(^neogeo[.]zip$)|(^funboot[.]rom$)|(^(ecs|exec|grom)[.]bin$)"

class ListGenerator : public std::vector<dir_discHdr>
{
public:
	void Init(const char *settingsDir, const char *Language);
	void Clear();
	void CreateListFromConsolizer();
	u32 Color;
	u32 Magic;
private:
	void OpenConfigs();
	void CloseConfigs();
	string gameTDB_Path;
	string CustomTitlesPath;
	string gameTDB_Language;
};

extern ListGenerator m_cacheList;

/* File enumeration — used by video.cpp, MusicPlayer.cpp, plugin.cpp, etc. */
typedef void (*FileAdder)(char *Path);
void GetFiles(const char *Path, const std::vector<string>& FileTypes,
			FileAdder AddFile, bool CompareFolders, u32 max_depth = 2, u32 depth = 1);

#endif /*_LISTGENERATOR_HPP_*/
