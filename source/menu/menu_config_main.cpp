
#include <algorithm>
#include "menu.hpp"

u8 mainCfg_Pages = 4;
u8 mainCfg_curPage = 1;

template <class T> static inline T loopNum(T i, T s)
{
	return (i + s) % s;
}

/* page 1 stuff */
vector<string> languages_available;
void AddLanguage(char *Path)
{
	char lng[32];
	memset(lng, 0, 32);
	char *lang_chr = strrchr(Path, '/')+1;
	memcpy(lng, lang_chr, min(31u, (u32)(strrchr(lang_chr, '.')-lang_chr)));
	languages_available.push_back(lng);
}

void CMenu::listThemes(const char * path, vector<string> &themes)
{
	DIR *d;
	struct dirent *dir;
	bool def = false;

	themes.clear();
	d = opendir(path);
	if(d != 0)
	{
		dir = readdir(d);
		while(dir != 0)
		{
			string fileName = dir->d_name;
			def = def || (upperCase(fileName) == "DEFAULT.INI");
			if(fileName.size() > 4 && fileName.substr(fileName.size() - 4, 4) == ".ini")
				themes.push_back(fileName.substr(0, fileName.size() - 4));
			dir = readdir(d);
		}
		closedir(d);
	}
	if(!def)
		themes.push_back("Default");
	sort(themes.begin(), themes.end());
}

void CMenu::_hideConfigButtons(bool instant)
{
	m_btnMgr.hide(m_configLbl1, instant);
	m_btnMgr.hide(m_configBtn1, instant);
	m_btnMgr.hide(m_configLbl2, instant);
	m_btnMgr.hide(m_configBtn2, instant);
	m_btnMgr.hide(m_configLbl3, instant);
	m_btnMgr.hide(m_configBtn3, instant);
	m_btnMgr.hide(m_configLbl4, instant);
	m_btnMgr.hide(m_configBtn4, instant);

	m_btnMgr.hide(m_configLbl1Val, instant);
	m_btnMgr.hide(m_configBtn1M, instant);
	m_btnMgr.hide(m_configBtn1P, instant);

	m_btnMgr.hide(m_configLbl2Val, instant);
	m_btnMgr.hide(m_configBtn2M, instant);
	m_btnMgr.hide(m_configBtn2P, instant);

	m_btnMgr.hide(m_configLbl3Val, instant);
	m_btnMgr.hide(m_configBtn3M, instant);
	m_btnMgr.hide(m_configBtn3P, instant);

	m_btnMgr.hide(m_configLbl4Val, instant);
	m_btnMgr.hide(m_configBtn4M, instant);
	m_btnMgr.hide(m_configBtn4P, instant);
}

void CMenu::_hideConfigMain(bool instant)
{
	m_btnMgr.hide(m_configLblTitle, instant);
	m_btnMgr.hide(m_configBtnBack, instant);
	m_btnMgr.hide(m_configLblPage, instant);
	m_btnMgr.hide(m_configBtnPageM, instant);
	m_btnMgr.hide(m_configBtnPageP, instant);
	for(u8 i = 0; i < ARRAY_SIZE(m_configLblUser); ++i)
		if(m_configLblUser[i] != -1)
			m_btnMgr.hide(m_configLblUser[i], instant);

	_hideConfigButtons(instant);
}

/*
 * Page layout (4 pages):
 *
 * Page 1 - Theme & Display:
 *   1. Theme (rotary)
 *   2. WiiFlow Language (rotary)
 *   3. Adjust Coverflow (button)
 *   4. Wiimote gestures (toggle)
 *
 * Page 2 - Covers:
 *   1. Download covers & banners (button)
 *   2. Covers Box Mode (toggle)
 *   3. Use HQ covers (toggle)
 *   4. Lock coverflow layouts (toggle)
 *
 * Page 3 - Volumes:
 *   1. Music volume (rotary 0-255)
 *   2. GUI sound volume (rotary 0-255)
 *   3. Coverflow sound volume (rotary 0-255)
 *   4. Game sound volume (rotary 0-255)
 *
 * Page 4 - Music & Sound:
 *   1. Randomize music (toggle)
 *   2. Music fade rate (rotary)
 *   3. Play GC banner sound (toggle)
 *   4. Play GC default sound (toggle)
 */

void CMenu::_showConfigMain()
{
	_setBg(m_configBg, m_configBg);
	m_btnMgr.show(m_configLblTitle);
	m_btnMgr.show(m_configBtnBack);
	for(u8 i = 0; i < ARRAY_SIZE(m_configLblUser); ++i)
		if(m_configLblUser[i] != -1)
			m_btnMgr.show(m_configLblUser[i]);

	m_btnMgr.show(m_configLblPage);
	m_btnMgr.show(m_configBtnPageM);
	m_btnMgr.show(m_configBtnPageP);
	m_btnMgr.setText(m_configLblPage, wfmt(L"%i / %i", mainCfg_curPage, mainCfg_Pages));

	_hideConfigButtons(true);

	m_btnMgr.show(m_configLbl1);
	m_btnMgr.show(m_configLbl2);
	m_btnMgr.show(m_configLbl3);
	m_btnMgr.show(m_configLbl4);

	if(mainCfg_curPage == 1)
	{
		/* Page 1: Theme & Display */
		m_btnMgr.show(m_configLbl1Val);
		m_btnMgr.show(m_configBtn1M);
		m_btnMgr.show(m_configBtn1P);
		m_btnMgr.show(m_configLbl2Val);
		m_btnMgr.show(m_configBtn2M);
		m_btnMgr.show(m_configBtn2P);
		m_btnMgr.show(m_configBtn3);
		m_btnMgr.show(m_configBtn4);

		m_btnMgr.setText(m_configLbl1, _t("cfga7", L"Theme"));
		m_btnMgr.setText(m_configLbl2, _t("cfgc9", L"WiiFlow Language"));
		m_btnMgr.setText(m_configLbl3, _t("cfgc4", L"Adjust Coverflow"));
		m_btnMgr.setText(m_configLbl4, _t("cfg710", L"Wiimote gestures"));

		m_btnMgr.setText(m_configLbl1Val, m_cfg.getString("GENERAL", "theme"));
		m_btnMgr.setText(m_configLbl2Val, m_curLanguage);
		m_btnMgr.setText(m_configBtn3, _t("cfgc5", L"Go"));
		m_btnMgr.setText(m_configBtn4, m_cfg.getBool("GENERAL", "wiimote_gestures") ? _t("on", L"On") : _t("off", L"Off"));
	}
	else if(mainCfg_curPage == 2)
	{
		/* Page 2: Covers */
		m_btnMgr.show(m_configBtn1);
		m_btnMgr.show(m_configBtn2);
		m_btnMgr.show(m_configBtn3);
		m_btnMgr.show(m_configBtn4);

		m_btnMgr.setText(m_configLbl1, _t("cfg3", L"Download covers & banners"));
		m_btnMgr.setText(m_configLbl2, _t("cfg726", L"Covers Box Mode"));
		m_btnMgr.setText(m_configLbl3, _t("cfg713", L"Use HQ covers"));
		m_btnMgr.setText(m_configLbl4, _t("cfg724", L"Lock coverflow layouts"));

		m_btnMgr.setText(m_configBtn1, _t("cfgc5", L"Go"));
		m_btnMgr.setText(m_configBtn2, m_cfg.getBool("general", "box_mode", false) ? _t("on", L"On") : _t("off", L"Off"));
		m_btnMgr.setText(m_configBtn3, m_cfg.getBool("GENERAL", "cover_use_hq") ? _t("yes", L"Yes") : _t("no", L"No"));
		m_btnMgr.setText(m_configBtn4, m_cfg.getBool("general", "cf_locked") ? _t("yes", L"Yes") : _t("no", L"No"));
	}
	else if(mainCfg_curPage == 3)
	{
		/* Page 3: Volumes */
		m_btnMgr.show(m_configLbl1Val);
		m_btnMgr.show(m_configBtn1M);
		m_btnMgr.show(m_configBtn1P);
		m_btnMgr.show(m_configLbl2Val);
		m_btnMgr.show(m_configBtn2M);
		m_btnMgr.show(m_configBtn2P);
		m_btnMgr.show(m_configLbl3Val);
		m_btnMgr.show(m_configBtn3M);
		m_btnMgr.show(m_configBtn3P);
		m_btnMgr.show(m_configLbl4Val);
		m_btnMgr.show(m_configBtn4M);
		m_btnMgr.show(m_configBtn4P);

		m_btnMgr.setText(m_configLbl1, _t("cfgs1", L"Music volume"));
		m_btnMgr.setText(m_configLbl1Val, wfmt(L"%i", m_cfg.getInt("GENERAL", "sound_volume_music", 255)));
		m_btnMgr.setText(m_configLbl2, _t("cfgs2", L"GUI sound volume"));
		m_btnMgr.setText(m_configLbl2Val, wfmt(L"%i", m_cfg.getInt("GENERAL", "sound_volume_gui", 255)));
		m_btnMgr.setText(m_configLbl3, _t("cfgs3", L"Coverflow sound volume"));
		m_btnMgr.setText(m_configLbl3Val, wfmt(L"%i", m_cfg.getInt("GENERAL", "sound_volume_coverflow", 255)));
		m_btnMgr.setText(m_configLbl4, _t("cfgs4", L"Game sound volume"));
		m_btnMgr.setText(m_configLbl4Val, wfmt(L"%i", m_cfg.getInt("GENERAL", "sound_volume_bnr", 255)));
	}
	else if(mainCfg_curPage == 4)
	{
		/* Page 4: Music & Sound */
		m_btnMgr.show(m_configBtn1);
		m_btnMgr.show(m_configLbl2Val);
		m_btnMgr.show(m_configBtn2M);
		m_btnMgr.show(m_configBtn2P);
		m_btnMgr.show(m_configBtn3);
		m_btnMgr.show(m_configBtn4);

		m_btnMgr.setText(m_configLbl1, _t("cfg715", L"Randomize music"));
		m_btnMgr.setText(m_configBtn1, m_cfg.getBool("GENERAL", "randomize_music") ? _t("yes", L"Yes") : _t("no", L"No"));
		m_btnMgr.setText(m_configLbl2, _t("cfg716", L"Music fade rate"));
		m_btnMgr.setText(m_configLbl2Val, wfmt(L"%i", m_cfg.getInt("GENERAL", "music_fade_rate", 8)));
		m_btnMgr.setText(m_configLbl3, _t("cfg720", L"Play GC banner sound"));
		m_btnMgr.setText(m_configBtn3, m_cfg.getBool(GC_DOMAIN, "play_banner_sound") ? _t("yes", L"Yes") : _t("no", L"No"));
		m_btnMgr.setText(m_configLbl4, _t("cfg721", L"Play GC default sound"));
		m_btnMgr.setText(m_configBtn4, m_cfg.getBool(GC_DOMAIN, "play_default_sound") ? _t("yes", L"Yes") : _t("no", L"No"));
	}
}

void CMenu::_configMain(void)
{
	mainCfg_curPage = 1;
	int val;
	bool rand_music = m_cfg.getBool("GENERAL", "randomize_music");
	bool hq_covers = m_cfg.getBool("GENERAL", "cover_use_hq");
	bool box_mode = m_cfg.getBool("GENERAL", "box_mode", true);
	u32 curLanguage = 0;
	int curTheme = 0;
	vector<string> themes;
	string prevLanguage;

	string prevTheme = m_cfg.getString("GENERAL", "theme");
	listThemes(m_themeDir.c_str(), themes);
	for(u32 i = 0; i < themes.size(); ++i)
	{
		if(themes[i] == prevTheme)
		{
			curTheme = i;
			break;
		}
	}

	languages_available.clear();
	languages_available.push_back("Default");
	GetFiles(m_languagesDir.c_str(), stringToVector(".ini", '|'), AddLanguage, false, 0);
	sort(languages_available.begin(), languages_available.end());

	for(u32 i = 0; i < languages_available.size(); ++i)
	{
		if(m_curLanguage == languages_available[i])
		{
			curLanguage = i;
			break;
		}
	}
	prevLanguage = languages_available[curLanguage];
	_showConfigMain();

	while(!m_exit)
	{
		_mainLoopCommon();
		if(BTN_HOME_PRESSED || BTN_B_PRESSED || (BTN_A_PRESSED && m_btnMgr.selected(m_configBtnBack)))
			break;
		else if(BTN_UP_PRESSED)
			m_btnMgr.up();
		else if(BTN_DOWN_PRESSED)
			m_btnMgr.down();
		else if(BTN_LEFT_PRESSED || BTN_MINUS_PRESSED || (BTN_A_PRESSED && m_btnMgr.selected(m_configBtnPageM)))
		{
			if(BTN_LEFT_PRESSED || BTN_MINUS_PRESSED)
				m_btnMgr.click(m_configBtnPageM);
			mainCfg_curPage -= 1;
			if(mainCfg_curPage < 1)
				mainCfg_curPage = mainCfg_Pages;
			_showConfigMain();
		}
		else if(BTN_RIGHT_PRESSED || BTN_PLUS_PRESSED || (BTN_A_PRESSED && m_btnMgr.selected(m_configBtnPageP)))
		{
			if(BTN_RIGHT_PRESSED || BTN_PLUS_PRESSED)
				m_btnMgr.click(m_configBtnPageP);
			mainCfg_curPage += 1;
			if(mainCfg_curPage > mainCfg_Pages)
				mainCfg_curPage = 1;
			_showConfigMain();
		}
		else if(BTN_A_PRESSED)
		{
			if(mainCfg_curPage == 1)
			{
				/* Page 1: Theme & Display */
				if(m_btnMgr.selected(m_configBtn1P) || m_btnMgr.selected(m_configBtn1M))
				{
					s8 direction = m_btnMgr.selected(m_configBtn1P) ? 1 : -1;
					curTheme = loopNum(curTheme + direction, (int)themes.size());
					m_themeName = themes[curTheme];
					m_cfg.setString("GENERAL", "theme", m_themeName);
					m_btnMgr.setText(m_configLbl1Val, m_cfg.getString("GENERAL", "theme"));
				}
				else if(m_btnMgr.selected(m_configBtn2P) || m_btnMgr.selected(m_configBtn2M))
				{
					s8 direction = m_btnMgr.selected(m_configBtn2P) ? 1 : -1;
					if(languages_available.size() > 1)
					{
						m_loc.unload();
						curLanguage = loopNum(curLanguage + direction, (u32)languages_available.size());
						m_curLanguage = languages_available[curLanguage];
						if(m_curLanguage != "Default")
							m_loc.load(fmt("%s/%s.ini", m_languagesDir.c_str(), m_curLanguage.c_str()));
						m_cfg.setString("GENERAL", "language", m_curLanguage.c_str());
						_updateText();
						_showConfigMain();
					}
				}
				else if(m_btnMgr.selected(m_configBtn3))
				{
					m_refreshGameList = true;
					_hideConfigMain();
					_cfTheme();
					_showConfigMain();
				}
				else if(m_btnMgr.selected(m_configBtn4))
				{
					m_cfg.setBool("GENERAL", "wiimote_gestures", !m_cfg.getBool("GENERAL", "wiimote_gestures"));
					m_btnMgr.setText(m_configBtn4, m_cfg.getBool("GENERAL", "wiimote_gestures") ? _t("on", L"On") : _t("off", L"Off"));
					enable_wmote_roll = m_cfg.getBool("GENERAL", "wiimote_gestures");
				}
			}
			if(mainCfg_curPage == 2)
			{
				/* Page 2: Covers */
				if(m_btnMgr.selected(m_configBtn1))
				{
					_hideConfigMain();
					_download();
					_showConfigMain();
				}
				else if(m_btnMgr.selected(m_configBtn2))
				{
					bool val = !m_cfg.getBool("general", "box_mode");
					m_cfg.setBool("general", "box_mode", val);
					m_btnMgr.setText(m_configBtn2, val ? _t("on", L"On") : _t("off", L"Off"));
				}
				else if(m_btnMgr.selected(m_configBtn3))
				{
					m_cfg.setBool("GENERAL", "cover_use_hq", !m_cfg.getBool("GENERAL", "cover_use_hq"));
					m_btnMgr.setText(m_configBtn3, m_cfg.getBool("GENERAL", "cover_use_hq") ? _t("yes", L"Yes") : _t("no", L"No"));
				}
				else if(m_btnMgr.selected(m_configBtn4))
				{
					bool val = !m_cfg.getBool("general", "cf_locked");
					m_cfg.setBool("general", "cf_locked", val);
					m_btnMgr.setText(m_configBtn4, val ? _t("yes", L"Yes") : _t("no", L"No"));
					CFLocked = val;
				}
			}
			if(mainCfg_curPage == 3)
			{
				/* Page 3: Volumes */
				if(m_btnMgr.selected(m_configBtn1P) || m_btnMgr.selected(m_configBtn1M))
				{
					s8 direction = m_btnMgr.selected(m_configBtn1P) ? 1 : -1;
					val = m_cfg.getInt("GENERAL", "sound_volume_music") + direction;
					if(val >= 0 && val < 256)
						m_cfg.setInt("GENERAL", "sound_volume_music", val);
					m_btnMgr.setText(m_configLbl1Val, wfmt(L"%i", m_cfg.getInt("GENERAL", "sound_volume_music")));
					MusicPlayer.SetMaxVolume(m_cfg.getInt("GENERAL", "sound_volume_music"));
				}
				else if(m_btnMgr.selected(m_configBtn2P) || m_btnMgr.selected(m_configBtn2M))
				{
					s8 direction = m_btnMgr.selected(m_configBtn2P) ? 1 : -1;
					val = m_cfg.getInt("GENERAL", "sound_volume_gui") + direction;
					if(val >= 0 && val < 256)
						m_cfg.setInt("GENERAL", "sound_volume_gui", val);
					m_btnMgr.setText(m_configLbl2Val, wfmt(L"%i", m_cfg.getInt("GENERAL", "sound_volume_gui")));
				}
				else if(m_btnMgr.selected(m_configBtn3P) || m_btnMgr.selected(m_configBtn3M))
				{
					s8 direction = m_btnMgr.selected(m_configBtn3P) ? 1 : -1;
					val = m_cfg.getInt("GENERAL", "sound_volume_coverflow") + direction;
					if(val >= 0 && val < 256)
						m_cfg.setInt("GENERAL", "sound_volume_coverflow", val);
					m_btnMgr.setText(m_configLbl3Val, wfmt(L"%i", m_cfg.getInt("GENERAL", "sound_volume_coverflow")));
					CoverFlow.setSoundVolume(m_cfg.getInt("GENERAL", "sound_volume_coverflow"));
				}
				else if(m_btnMgr.selected(m_configBtn4P) || m_btnMgr.selected(m_configBtn4M))
				{
					s8 direction = m_btnMgr.selected(m_configBtn4P) ? 1 : -1;
					val = m_cfg.getInt("GENERAL", "sound_volume_bnr") + direction;
					if(val >= 0 && val < 256)
						m_cfg.setInt("GENERAL", "sound_volume_bnr", val);
					m_btnMgr.setText(m_configLbl4Val, wfmt(L"%i", m_cfg.getInt("GENERAL", "sound_volume_bnr")));
					m_bnrSndVol = m_cfg.getInt("GENERAL", "sound_volume_bnr");
				}
			}
			if(mainCfg_curPage == 4)
			{
				/* Page 4: Music & Sound */
				if(m_btnMgr.selected(m_configBtn1))
				{
					m_cfg.setBool("GENERAL", "randomize_music", !m_cfg.getBool("GENERAL", "randomize_music"));
					m_btnMgr.setText(m_configBtn1, m_cfg.getBool("GENERAL", "randomize_music") ? _t("yes", L"Yes") : _t("no", L"No"));
				}
				else if(m_btnMgr.selected(m_configBtn2P) || m_btnMgr.selected(m_configBtn2M))
				{
					s8 direction = m_btnMgr.selected(m_configBtn2P) ? 1 : -1;
					val = m_cfg.getInt("GENERAL", "music_fade_rate") + direction;
					if(val >= 0 && val < 30)
						m_cfg.setInt("GENERAL", "music_fade_rate", val);
					m_btnMgr.setText(m_configLbl2Val, wfmt(L"%i", m_cfg.getInt("GENERAL", "music_fade_rate")));
					MusicPlayer.SetFadeRate(m_cfg.getInt("GENERAL", "music_fade_rate"));
				}
				else if(m_btnMgr.selected(m_configBtn3))
				{
					m_cfg.setBool(GC_DOMAIN, "play_banner_sound", !m_cfg.getBool(GC_DOMAIN, "play_banner_sound"));
					m_btnMgr.setText(m_configBtn3, m_cfg.getBool(GC_DOMAIN, "play_banner_sound") ? _t("yes", L"Yes") : _t("no", L"No"));
					m_gc_play_banner_sound = m_cfg.getBool(GC_DOMAIN, "play_banner_sound", true);
				}
				else if(m_btnMgr.selected(m_configBtn4))
				{
					m_cfg.setBool(GC_DOMAIN, "play_default_sound", !m_cfg.getBool(GC_DOMAIN, "play_default_sound"));
					m_btnMgr.setText(m_configBtn4, m_cfg.getBool(GC_DOMAIN, "play_default_sound") ? _t("yes", L"Yes") : _t("no", L"No"));
					m_gc_play_default_sound = m_cfg.getBool(GC_DOMAIN, "play_default_sound", true);
				}
			}
		}
	}
	if(m_curLanguage != prevLanguage)
	{
		m_cacheList.Init(m_settingsDir.c_str(), m_loc.getString(m_curLanguage, "gametdb_code", "EN").c_str());
		fsop_deleteFolder(m_listCacheDir.c_str());
		fsop_MakeFolder(m_listCacheDir.c_str());
		m_refreshGameList = true;
	}
	if(rand_music != m_cfg.getBool("GENERAL", "randomize_music"))
		MusicPlayer.Init(m_cfg, m_musicDir, fmt("%s/music", m_themeDataDir.c_str()));
	if(!m_refreshGameList && (hq_covers != m_cfg.getBool("GENERAL", "cover_use_hq") || box_mode != m_cfg.getBool("general", "box_mode")))
		_initCF();
	_hideConfigMain();
}

void CMenu::_initConfigMenu()
{
	m_configBg = _texture("CONFIG/BG", "texture", theme.bg, false);

	_addUserLabels(m_configLblUser, ARRAY_SIZE(m_configLblUser), "CONFIG");
	m_configLblTitle = _addLabel("CONFIG/TITLE", theme.titleFont, L"", 0, 10, 640, 60, theme.titleFontColor, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE);
	m_configBtnBack = _addButton("CONFIG/BACK_BTN", theme.btnFont, L"", 420, 400, 200, 48, theme.btnFontColor);
	m_configLblPage = _addLabel("CONFIG/PAGE_BTN", theme.btnFont, L"", 68, 400, 104, 48, theme.btnFontColor, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE, theme.btnTexC);
	m_configBtnPageM = _addPicButton("CONFIG/PAGE_MINUS", theme.btnTexMinus, theme.btnTexMinusS, 20, 400, 48, 48);
	m_configBtnPageP = _addPicButton("CONFIG/PAGE_PLUS", theme.btnTexPlus, theme.btnTexPlusS, 172, 400, 48, 48);

	m_configLbl1 = _addLabel("CONFIG/LINE1", theme.lblFont, L"", 20, 125, 385, 56, theme.lblFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
	m_configBtn1 = _addButton("CONFIG/LINE1_BTN", theme.btnFont, L"", 420, 130, 200, 48, theme.btnFontColor);
	m_configLbl2 = _addLabel("CONFIG/LINE2", theme.lblFont, L"", 20, 185, 385, 56, theme.lblFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
	m_configBtn2 = _addButton("CONFIG/LINE2_BTN", theme.btnFont, L"", 420, 190, 200, 48, theme.btnFontColor);
	m_configLbl3 = _addLabel("CONFIG/LINE3", theme.lblFont, L"", 20, 245, 385, 56, theme.lblFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
	m_configBtn3 = _addButton("CONFIG/LINE3_BTN", theme.btnFont, L"", 420, 250, 200, 48, theme.btnFontColor);
	m_configLbl4 = _addLabel("CONFIG/LINE4", theme.lblFont, L"", 20, 305, 385, 56, theme.lblFontColor, FTGX_JUSTIFY_LEFT | FTGX_ALIGN_MIDDLE);
	m_configBtn4 = _addButton("CONFIG/LINE4_BTN", theme.btnFont, L"", 420, 310, 200, 48, theme.btnFontColor);

	m_configLbl1Val = _addLabel("CONFIG/LINE1_VAL", theme.btnFont, L"", 468, 130, 104, 48, theme.btnFontColor, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE, theme.btnTexC);
	m_configBtn1M = _addPicButton("CONFIG/LINE1_MINUS", theme.btnTexMinus, theme.btnTexMinusS, 420, 130, 48, 48);
	m_configBtn1P = _addPicButton("CONFIG/LINE1_PLUS", theme.btnTexPlus, theme.btnTexPlusS, 572, 130, 48, 48);
	m_configLbl2Val = _addLabel("CONFIG/LINE2_VAL", theme.btnFont, L"", 468, 190, 104, 48, theme.btnFontColor, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE, theme.btnTexC);
	m_configBtn2M = _addPicButton("CONFIG/LINE2_MINUS", theme.btnTexMinus, theme.btnTexMinusS, 420, 190, 48, 48);
	m_configBtn2P = _addPicButton("CONFIG/LINE2_PLUS", theme.btnTexPlus, theme.btnTexPlusS, 572, 190, 48, 48);
	m_configLbl3Val = _addLabel("CONFIG/LINE3_VAL", theme.btnFont, L"", 468, 250, 104, 48, theme.btnFontColor, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE, theme.btnTexC);
	m_configBtn3M = _addPicButton("CONFIG/LINE3_MINUS", theme.btnTexMinus, theme.btnTexMinusS, 420, 250, 48, 48);
	m_configBtn3P = _addPicButton("CONFIG/LINE3_PLUS", theme.btnTexPlus, theme.btnTexPlusS, 572, 250, 48, 48);
	m_configLbl4Val = _addLabel("CONFIG/LINE4_VAL", theme.btnFont, L"", 468, 310, 104, 48, theme.btnFontColor, FTGX_JUSTIFY_CENTER | FTGX_ALIGN_MIDDLE, theme.btnTexC);
	m_configBtn4M = _addPicButton("CONFIG/LINE4_MINUS", theme.btnTexMinus, theme.btnTexMinusS, 420, 310, 48, 48);
	m_configBtn4P = _addPicButton("CONFIG/LINE4_PLUS", theme.btnTexPlus, theme.btnTexPlusS, 572, 310, 48, 48);

	_setHideAnim(m_configLbl1, "CONFIG/LINE1", 50, 0, -2.f, 0.f);
	_setHideAnim(m_configBtn1, "CONFIG/LINE1_BTN", -50, 0, 1.f, 0.f);
	_setHideAnim(m_configLbl2, "CONFIG/LINE2", 50, 0, -2.f, 0.f);
	_setHideAnim(m_configBtn2, "CONFIG/LINE2_BTN", -50, 0, 1.f, 0.f);
	_setHideAnim(m_configLbl3, "CONFIG/LINE3", 50, 0, -2.f, 0.f);
	_setHideAnim(m_configBtn3, "CONFIG/LINE3_BTN", -50, 0, 1.f, 0.f);
	_setHideAnim(m_configLbl4, "CONFIG/LINE4", 50, 0, -2.f, 0.f);
	_setHideAnim(m_configBtn4, "CONFIG/LINE4_BTN", -50, 0, 1.f, 0.f);

	_setHideAnim(m_configLbl1Val, "CONFIG/LINE1_VAL", -50, 0, 1.f, 0.f);
	_setHideAnim(m_configBtn1M, "CONFIG/LINE1_MINUS", -50, 0, 1.f, 0.f);
	_setHideAnim(m_configBtn1P, "CONFIG/LINE1_PLUS", -50, 0, 1.f, 0.f);
	_setHideAnim(m_configLbl2Val, "CONFIG/LINE2_VAL", -50, 0, 1.f, 0.f);
	_setHideAnim(m_configBtn2M, "CONFIG/LINE2_MINUS", -50, 0, 1.f, 0.f);
	_setHideAnim(m_configBtn2P, "CONFIG/LINE2_PLUS", -50, 0, 1.f, 0.f);
	_setHideAnim(m_configLbl3Val, "CONFIG/LINE3_VAL", -50, 0, 1.f, 0.f);
	_setHideAnim(m_configBtn3M, "CONFIG/LINE3_MINUS", -50, 0, 1.f, 0.f);
	_setHideAnim(m_configBtn3P, "CONFIG/LINE3_PLUS", -50, 0, 1.f, 0.f);
	_setHideAnim(m_configLbl4Val, "CONFIG/LINE4_VAL", -50, 0, 1.f, 0.f);
	_setHideAnim(m_configBtn4M, "CONFIG/LINE4_MINUS", -50, 0, 1.f, 0.f);
	_setHideAnim(m_configBtn4P, "CONFIG/LINE4_PLUS", -50, 0, 1.f, 0.f);

	_setHideAnim(m_configLblTitle, "CONFIG/TITLE", 0, 0, -2.f, 0.f);
	_setHideAnim(m_configBtnBack, "CONFIG/BACK_BTN", 0, 0, 1.f, -1.f);
	_setHideAnim(m_configLblPage, "CONFIG/PAGE_BTN", 0, 0, 1.f, -1.f);
	_setHideAnim(m_configBtnPageM, "CONFIG/PAGE_MINUS", 0, 0, 1.f, -1.f);
	_setHideAnim(m_configBtnPageP, "CONFIG/PAGE_PLUS", 0, 0, 1.f, -1.f);

	_hideConfigMain(true);
	_textConfig();
}

void CMenu::_textConfig(void)
{
	m_btnMgr.setText(m_configLblTitle, _t("cfg1", L"Settings"));
	m_btnMgr.setText(m_configBtnBack, _t("cfg10", L"Back"));
}
