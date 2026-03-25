
#include <ogc/system.h>
#include <unistd.h>
#include <sys/stat.h>

#include "defines.h"
#include "channel/nand.hpp"
#include "devicemounter/DeviceHandler.hpp"
#include "gecko/gecko.hpp"
#include "gui/video.hpp"
#include "gui/text.hpp"
#include "loader/sys.h"
#include "menu/menu.hpp"
#include "memory/memory.h"

bool isWiiVC = false;
bool sdOnly = true;
volatile bool NANDemuView = false;
volatile bool networkInit = false;

int main(int argc, char **argv)
{
	MEM_init();
	__exception_setreload(10);
	Gecko_Init();
	#ifdef COMMITHASH
		gprintf(" \nWelcome to %s %s %s!\nThis is the debug output.\n", APP_NAME, APP_VERSION, COMMITHASH);
	#else
		gprintf(" \nWelcome to %s %s!\nThis is the debug output.\n", APP_NAME, APP_VERSION);
	#endif

	char gameid[6];
	memset(&gameid, 0, sizeof(gameid));
	bool wait_loop = false;
	char wait_dir[256];
	memset(&wait_dir, 0, sizeof(wait_dir));

	for(u8 i = 0; i < argc; i++)
	{
		if(strcasestr(argv[i], "waitdir=") != NULL)
		{
			char *ptr = strcasestr(argv[i], "waitdir=");
			strncpy(wait_dir, ptr+strlen("waitdir="), sizeof(wait_dir) - 1);
		}
		else if(strcasestr(argv[i], "Waitloop") != NULL)
			wait_loop = true;
		else if(strlen(argv[i]) == 6 || strlen(argv[i]) == 4)
		{
			strcpy(gameid, argv[i]);
			for(u8 j = 0; j < strlen(gameid) - 1; j++)
			{
				if(!isalnum(gameid[j]))
				{
					gameid[0] = 0;
					break;
				}
			}
		}
	}

	/* Init video */
	m_vid.init();

	/* Init device partition handlers */
	DeviceHandle.Init();

	/* Init NAND handlers */
	NandHandle.Init();
	NandHandle.Init_ISFS();

	/* Mount SD */
	bool sd_mounted = DeviceHandle.MountSD();
	gprintf("SD mount: %s\n", sd_mounted ? "OK" : "FAILED");
	gprintf("SD inserted: %s\n", DeviceHandle.IsInserted(SD) ? "YES" : "NO");

	/* Init wait images and show wait animation */
	if(strlen(gameid) == 0)
	{
		m_vid.setCustomWaitImgs(wait_dir, wait_loop);
		m_vid.waitMessage(0.15f);
	}

	/* Init controllers for input */
	Open_Inputs();

	/* sys inits */
	Sys_Init();

	bool startup_successful = false;
	/* Init configs, folders, coverflow, gui and more */
	if(mainMenu.init())
	{
		startup_successful = true;
		if(strlen(gameid) != 0)
			mainMenu.directlaunch(gameid);
		else
			mainMenu.main();
	}
	else
	{
		/* init() failed — show error on a basic console, wait, then exit. */
		gprintf("CMenu::init() failed\n");
		gprintf("SD mounted: %s\n", sd_mounted ? "YES" : "NO");
		gprintf("SD inserted: %s\n", DeviceHandle.IsInserted(SD) ? "YES" : "NO");

		/* Cleanly shut down WiiFlow's GX video before re-initing console */
		m_vid.cleanup();

		/* Re-init basic console on framebuffer for error display */
		VIDEO_Init();
		GXRModeObj *rmode = VIDEO_GetPreferredMode(NULL);
		void *xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
		VIDEO_Configure(rmode);
		VIDEO_SetNextFramebuffer(xfb);
		VIDEO_SetBlack(FALSE);
		VIDEO_Flush();
		VIDEO_WaitVSync();
		if(rmode->viTVMode & VI_NON_INTERLACE)
			VIDEO_WaitVSync();
		CON_Init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight,
		         rmode->fbWidth * VI_DISPLAY_PIX_SZ);

		printf("\n\n");
		printf("  *** WiiFlow Init Failed ***\n\n");
		printf("  SD card config not found.\n");
		printf("  WiiFlow needs sd:/apps/wiiflow/ with\n");
		printf("  config files and theme data.\n\n");
		printf("  SD mounted: %s\n", sd_mounted ? "YES" : "NO");
		printf("  SD inserted: %s\n", DeviceHandle.IsInserted(SD) ? "YES" : "NO");
		printf("\n  Halted. Close Dolphin to exit.\n");

		for(;;)
			VIDEO_WaitVSync();
	}

	/* Shutdown */
	Close_Inputs();
	DeviceHandle.UnMountAll();
	if(startup_successful)
		Sys_Exit();
	return 0;
}
