/* Stub definitions for variables declared in stub headers.
 * Not used in Dolphin Consolizer build — just keeps the linker happy. */
#include <gctypes.h>

/* From alt_ios.h */
u8 mainIOS = 58;
bool useMainIOS = false;

/* From cios.h */
typedef struct { u32 Version; u32 Base; } IOS_Info;
IOS_Info CurrentIOS = { 58, 58 };

/* From wbfs.h */
char wii_games_dir[64] = "%s:/wbfs";
char gc_games_dir[64] = "%s:/games";

/* From deleted USB device handler */
int currentPort = 0;
int currentPartition = 0;
