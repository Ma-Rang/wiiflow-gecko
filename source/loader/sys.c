
#include <ogc/lwp_threads.h>
#include <ogc/pad.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys.h"
#include "gecko/gecko.hpp"
#include "memory/mem2.hpp"
#include "memory/memory.h"
#include "sicksaxis-wrapper/sicksaxis-wrapper.h"
#include "wiiuse/wpad.h"
#include "wupc/wupc.h"

/* Variables */
bool reset = false;
bool shutdown = false;
volatile u8 ExitOption = 0;

void __Wpad_PowerCallback(s32 chan)
{
	(void)chan;
	shutdown = 1;
}

void Open_Inputs(void)
{
	PAD_Init();
	WUPC_Init();
	WPAD_Init();
	DS3_Init();

	WPAD_SetPowerButtonCallback(__Wpad_PowerCallback);
	WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetIdleTimeout(60 * 2);
}

void Close_Inputs(void)
{
	WUPC_Shutdown();

	for(u32 cnt = 0; cnt < 4; cnt++)
		WPAD_Disconnect(cnt);

	WPAD_Shutdown();
	DS3_Cleanup();
}

bool Sys_Exiting(void)
{
	DCFlushRange(&reset, 32);
	DCFlushRange(&shutdown, 32);
	return reset || shutdown;
}

int Sys_GetExitTo(void)
{
	return ExitOption;
}

void Sys_ExitTo(int option)
{
	ExitOption = option;
}

void Sys_Exit(void)
{
	Close_Inputs();

	if(ExitOption == SHUTDOWN_STANDBY)
		SYS_ResetSystem(SYS_POWEROFF_STANDBY, 0, 0);

	if(ExitOption == SHUTDOWN_IDLE)
		SYS_ResetSystem(SYS_POWEROFF_IDLE, 0, 0);

	/* Return to System Menu */
	SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	exit(0);
}

void __Sys_ResetCallback(u32 irq, void *ctx)
{
	(void)irq;
	(void)ctx;
	reset = true;
}

void __Sys_PowerCallback(void)
{
	shutdown = true;
}

void Sys_Init(void)
{
	SYS_SetResetCallback(__Sys_ResetCallback);
	SYS_SetPowerCallback(__Sys_PowerCallback);
}

/* KILL IT */
s32 __IOS_LoadStartupIOS() { return 0; }
