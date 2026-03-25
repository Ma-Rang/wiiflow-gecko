
#include <network.h>
#include "menu.hpp"
#include "devicemounter/DeviceHandler.hpp"
#include "consolizer/consolizer.hpp"
#include "gecko/gecko.hpp"
#include "loader/sys.h"
#include "music/SoundHandler.hpp"

void CMenu::_launch(const dir_discHdr *hdr)
{
	/* Stop music */
	MusicPlayer.Stop();

	/* Cleanup */
	_launchShutdown();

	/* Send boot token to Consolizer via USB Gecko */
	gprintf("Launching game: %.6s (token: %08x)\n", hdr->id, hdr->settings[0]);
	Consolizer_SendLaunch(hdr->settings[0]);

	/* Wait for Dolphin to boot the game (we get replaced) */
	for(;;)
		VIDEO_WaitVSync();
}

void CMenu::_launchShutdown(void)
{
	Close_Inputs();
	if(networkInit)
	{
		net_deinit();
		networkInit = false;
	}
	DeviceHandle.UnMountAll();
}

void CMenu::directlaunch(const char *gameid)
{
	for(u32 i = 0; i < m_gameList.size(); i++)
	{
		if(strncasecmp(m_gameList[i].id, gameid, 6) == 0)
		{
			_launch(&m_gameList[i]);
			return;
		}
	}
	gprintf("Game ID %.6s not found in game list\n", gameid);
}
