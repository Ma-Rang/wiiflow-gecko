/**
 * Stub implementations for deleted menu functions.
 * These were in menu_wbfs.cpp, menu_nandemu.cpp, menu_partitions.cpp,
 * menu_explorer.cpp, menu_config_gc.cpp, menu_config_hb.cpp, menu_wad.cpp
 * — all removed for the Dolphin Consolizer build.
 */
#include "menu.hpp"
#include "gc/gcdisc.hpp"

GC_Disc GC_Disc_Reader;

/* Config menus for removed features */
void CMenu::_configGC(void) {}
void CMenu::_ConfigHB() {}
int CMenu::_NandEmuCfg(void) { return 0; }
void CMenu::_partitionsCfg(void) {}
bool CMenu::_wbfsOp(WBFS_OP op) { (void)op; return false; }
void CMenu::_Explorer(void) {}
void CMenu::_Wad(const char *wad_path) { (void)wad_path; }

/* Removed launch methods — all games go through Consolizer */
void CMenu::_launchWii(dir_discHdr *hdr, bool dvd, bool disc_cfg) { (void)hdr; (void)dvd; (void)disc_cfg; _launch(hdr); }
void CMenu::_launchChannel(dir_discHdr *hdr) { (void)hdr; _launch(hdr); }
void CMenu::_launchHomebrew(const char *filepath, vector<string> arguments) { (void)filepath; (void)arguments; }
void CMenu::_launchGC(dir_discHdr *hdr, bool disc) { (void)hdr; (void)disc; _launch(hdr); }
void CMenu::_launchPlugin(dir_discHdr *hdr) { (void)hdr; _launch(hdr); }
int CMenu::_loadGameIOS(u8 ios, int userIOS, const char *id, bool RealNAND_Channels) { (void)ios; (void)userIOS; (void)id; (void)RealNAND_Channels; return 0; }
bool CMenu::_loadFile(u8 * &buffer, u32 &size, const char *path, const char *file) { (void)buffer; (void)size; (void)path; (void)file; return false; }
vector<string> CMenu::_getMetaXML(const char *bootpath) { (void)bootpath; return vector<string>(); }

/* Removed config game menus */
void CMenu::_configGCGame(const dir_discHdr *GameHdr, bool disc) { (void)GameHdr; (void)disc; }

/* Init/Text/Hide/Show stubs for deleted menu pages */
void CMenu::_initConfigGCMenu() {}
void CMenu::_initConfigHB() {}
void CMenu::_initNandEmuMenu() {}
void CMenu::_initPartitionsCfgMenu() {}
void CMenu::_initWBFSMenu() {}
void CMenu::_initExplorer() {}
void CMenu::_initWad() {}

void CMenu::_textConfigGC(void) {}
void CMenu::_textConfigHB(void) {}
void CMenu::_textNandEmu(void) {}
void CMenu::_textPartitionsCfg(void) {}
void CMenu::_textWBFS(void) {}
void CMenu::_textExplorer(void) {}
void CMenu::_textWad(void) {}

void CMenu::_hideConfigGC(bool instant) { (void)instant; }
void CMenu::_hideConfigHB(bool instant) { (void)instant; }
void CMenu::_hideNandEmu(bool instant) { (void)instant; }
void CMenu::_hidePartitionsCfg(bool instant) { (void)instant; }
void CMenu::_hideWBFS(bool instant) { (void)instant; }
void CMenu::_hideExplorer(bool instant) { (void)instant; }
void CMenu::_hideWad(bool instant) { (void)instant; }
void CMenu::_hideConfigGCGame(bool instant) { (void)instant; }

void CMenu::_showConfigGC(void) {}
void CMenu::_showConfigHB(void) {}
void CMenu::_showNandEmu(void) {}
void CMenu::_showPartitionsCfg(void) {}
void CMenu::_showWBFS(WBFS_OP op) { (void)op; }
void CMenu::_showExplorer(void) {}
void CMenu::_showWad(void) {}
void CMenu::_showConfigGCGame(void) {}

/* Nand emu functions */
int CMenu::_FindEmuPart(bool savesnand, bool searchvalid) { (void)savesnand; (void)searchvalid; return -1; }
bool CMenu::_checkSave(string id, int nand_type) { (void)id; (void)nand_type; return false; }
bool CMenu::_TestEmuNand(int epart, const char *path, bool indept) { (void)epart; (void)path; (void)indept; return false; }
void CMenu::_getEmuNands(void) {}
void CMenu::_FullNandCheck(void) {}
void CMenu::_listEmuNands(const char *path, vector<string> &nands) { (void)path; (void)nands; }
int CMenu::_ExtractGameSave(string gameId) { (void)gameId; return -1; }
int CMenu::_FlashGameSave(string gameId) { (void)gameId; return -1; }
void *CMenu::_NandDumper(void *obj) { (void)obj; return NULL; }
void *CMenu::_NandFlasher(void *obj) { (void)obj; return NULL; }

/* Explorer stubs */
const char *CMenu::_FolderExplorer(const char *startPath) { (void)startPath; return NULL; }
void CMenu::_wadExplorer(void) {}
void CMenu::_refreshExplorer(s8 direction) { (void)direction; }

/* GC disc dump / WBFS install stubs */
/* update_pThread is defined in menu_download.cpp */
void CMenu::GC_Messenger(int message, int info, char *cinfo) { (void)message; (void)info; (void)cinfo; }
void CMenu::_addDiscProgress(int status, int total, void *user_data) { (void)status; (void)total; (void)user_data; }
void CMenu::_ShowProgress(int dumpstat, int dumpprog, int filestat, int fileprog, int files, int folders, const char *tmess, void *user_data) { (void)dumpstat; (void)dumpprog; (void)filestat; (void)fileprog; (void)files; (void)folders; (void)tmess; (void)user_data; }
void *CMenu::_gameInstaller(void *obj) { (void)obj; return NULL; }
void *CMenu::_GCcopyGame(void *obj) { (void)obj; return NULL; }
bool CMenu::_searchGamesByID(const char *gameId) { (void)gameId; return false; }
int CMenu::_GCgameInstaller() { return -1; }

/* Cheat settings stubs */
void CMenu::_CheatSettings(void) {}
void CMenu::_initCheatSettingsMenu(void) {}
void CMenu::_textCheatSettings(void) {}
