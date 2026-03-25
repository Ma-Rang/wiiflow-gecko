
#ifndef _SYS_H_
#define _SYS_H_

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum
{
	EXIT_TO_MENU = 0,
	EXIT_TO_HBC,
	EXIT_TO_WIIU,
	EXIT_TO_PRIILOADER,
	EXIT_TO_BOOTMII,
	EXIT_TO_SMNK2O,
	WIIFLOW_DEF,
	SHUTDOWN_STANDBY,
	SHUTDOWN_IDLE,
};

/* Prototypes */
void Sys_Init(void);
bool Sys_Exiting(void);
void Sys_Exit(void);
void Sys_ExitTo(int);
int Sys_GetExitTo(void);

void Open_Inputs(void);
void Close_Inputs(void);

/* All our extern C stuff */
extern void __exception_setreload(int t);
extern bool sdOnly;
extern bool isWiiVC;
extern volatile bool NANDemuView;
extern volatile bool networkInit;

/* Stubs for removed cIOS/vWii functionality */
static inline bool IsOnWiiU(void) { return false; }
static inline bool AHBPROT_Patched(void) { return false; }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
