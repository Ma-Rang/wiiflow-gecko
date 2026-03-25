/**
 * Dolphin Consolizer — USB Gecko communication layer for WiiFlow.
 *
 * Protocol (Wii → Consolizer):
 *   0x10 HELLO         [1 byte: 0x02 = WiiFlow]
 *   0x12 REQUEST_LIST  [0 bytes]
 *   0x13 LAUNCH        [4 bytes: boot_token BE]
 *   0x14 REQUEST_SOUND [4 bytes: boot_token BE]
 *
 * Protocol (Consolizer → Wii):
 *   0x01 GAME_ENTRY   [11+ bytes: 6-byte game_id + 4-byte boot_token BE + 1-byte type
 *                       + 1-byte title_len + N-byte title (UTF-8, no null)]
 *   0x02 LIST_END     [0 bytes]
 *   0x05 SOUND_CHUNK  [up to 65535 bytes of sound.bin data]
 *   0x06 SOUND_END    [0 bytes — transfer complete]
 *   0x07 SOUND_NONE   [0 bytes — no sound available]
 *
 * Game types:
 *   0x00 = Wii game
 *   0x01 = GameCube game
 */
#ifndef _CONSOLIZER_HPP_
#define _CONSOLIZER_HPP_

#include <gctypes.h>

#define CONSOLIZER_TYPE_WII  0x00
#define CONSOLIZER_TYPE_GC   0x01
#define CONSOLIZER_TITLE_MAX 63

/* Per-game entry received from Consolizer */
typedef struct {
	char game_id[7]; /* 6 chars + null */
	u32  boot_token;  /* CRC32 of file path — opaque, sent back with LAUNCH */
	u8   type;        /* CONSOLIZER_TYPE_WII or CONSOLIZER_TYPE_GC */
	char title[CONSOLIZER_TITLE_MAX + 1]; /* friendly name from Consolizer */
} ConsolizerGameEntry;

/* Initialize the USB Gecko link to Consolizer.
 * Probes Slot B then Slot A. Returns true if Gecko found. */
bool Consolizer_Init(void);

/* Send HELLO to Consolizer. Returns true on success. */
bool Consolizer_Hello(void);

/* Request the game list from Consolizer.
 * Fills entries[] up to max_entries. Returns count, or -1 on error. */
int Consolizer_RequestGameList(ConsolizerGameEntry *entries, int max_entries);

/* Send LAUNCH command with the boot token. */
void Consolizer_SendLaunch(u32 boot_token);

/* Request banner sound.bin for a game from Consolizer.
 * Allocates buffer via malloc on success.
 * Returns sound.bin size, or 0 on failure/no sound.
 * Caller must free() *out_buf if return > 0. */
u32 Consolizer_RequestSound(u32 boot_token, u8 **out_buf);

#endif /* _CONSOLIZER_HPP_ */
