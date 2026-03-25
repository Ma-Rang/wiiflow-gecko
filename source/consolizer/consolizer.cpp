/**
 * Dolphin Consolizer — USB Gecko communication for WiiFlow.
 * Based on the Wii launcher DOL's Gecko I/O.
 */
#include <ogc/exi.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "consolizer.hpp"
#include "gecko/gecko.hpp"

/* EXI constants */
#define GECKO_DEVICE_ID  0x04700000
#define EXI_SLOT_B       1
#define EXI_SLOT_A       0

/* Protocol command bytes */
#define CMD_GAME_ENTRY   0x01
#define CMD_LIST_END     0x02
#define CMD_HELLO         0x10
#define CMD_REQUEST_LIST  0x12
#define CMD_LAUNCH        0x13
#define CMD_REQUEST_SOUND 0x14

#define CMD_SOUND_CHUNK   0x05
#define CMD_SOUND_END     0x06
#define CMD_SOUND_NONE    0x07

#define HELLO_TYPE_WIIFLOW 0x02

static int gecko_channel = -1;

/* ---------- Low-level Gecko I/O ---------- */

static bool gecko_probe(int channel)
{
	u32 id = 0;
	if(!EXI_Lock(channel, 0, NULL)) return false;
	if(!EXI_Select(channel, 0, EXI_SPEED8MHZ))
	{
		EXI_Unlock(channel);
		return false;
	}
	u32 cmd = 0x90000000;
	EXI_Imm(channel, &cmd, 4, EXI_READWRITE, NULL);
	EXI_Sync(channel);
	id = cmd;
	EXI_Deselect(channel);
	EXI_Unlock(channel);
	return (id == GECKO_DEVICE_ID);
}

static bool gecko_send_byte(u8 byte)
{
	for(int retries = 64; retries > 0; retries--)
	{
		if(!EXI_Lock(gecko_channel, 0, NULL)) return false;
		if(!EXI_Select(gecko_channel, 0, EXI_SPEED8MHZ))
		{
			EXI_Unlock(gecko_channel);
			return false;
		}
		u32 cmd = 0xB0000000 | ((u32)byte << 20);
		EXI_Imm(gecko_channel, &cmd, 4, EXI_READWRITE, NULL);
		EXI_Sync(gecko_channel);
		EXI_Deselect(gecko_channel);
		EXI_Unlock(gecko_channel);
		if(cmd & 0x04000000) return true;
	}
	return false;
}

static bool gecko_send(const void *data, u32 len)
{
	const u8 *p = (const u8 *)data;
	for(u32 i = 0; i < len; i++)
		if(!gecko_send_byte(p[i])) return false;
	return true;
}

static bool gecko_send_frame(u8 cmd, const void *payload, u16 len)
{
	u8 header[3];
	header[0] = cmd;
	header[1] = (u8)(len >> 8);
	header[2] = (u8)(len & 0xFF);
	if(!gecko_send(header, 3)) return false;
	if(len > 0 && payload)
		if(!gecko_send(payload, len)) return false;
	return true;
}

static int gecko_recv_byte(u8 *out)
{
	for(int retries = 1000; retries > 0; retries--)
	{
		if(!EXI_Lock(gecko_channel, 0, NULL)) return 0;
		if(!EXI_Select(gecko_channel, 0, EXI_SPEED8MHZ))
		{
			EXI_Unlock(gecko_channel);
			return 0;
		}
		u32 cmd = 0xA0000000;
		EXI_Imm(gecko_channel, &cmd, 4, EXI_READWRITE, NULL);
		EXI_Sync(gecko_channel);
		EXI_Deselect(gecko_channel);
		EXI_Unlock(gecko_channel);
		if(cmd & 0x08000000)
		{
			*out = (u8)((cmd >> 16) & 0xFF);
			return 1;
		}
	}
	return 0;
}

static bool gecko_recv(void *data, u32 len)
{
	u8 *p = (u8 *)data;
	for(u32 i = 0; i < len; i++)
		if(!gecko_recv_byte(&p[i])) return false;
	return true;
}

/* Read a frame: [cmd] [len_hi] [len_lo] [payload...]
 * Returns cmd byte, fills payload and sets *out_len. Returns 0 on error. */
static u8 gecko_recv_frame(void *payload, u16 max_len, u16 *out_len)
{
	u8 header[3];
	if(!gecko_recv(header, 3)) return 0;
	u16 len = ((u16)header[1] << 8) | header[2];
	if(out_len) *out_len = len;
	if(len > max_len)
	{
		/* Drain excess bytes */
		u8 dummy;
		for(u16 i = 0; i < len; i++) gecko_recv_byte(&dummy);
		return 0;
	}
	if(len > 0 && payload)
		if(!gecko_recv(payload, len)) return 0;
	return header[0];
}

/* ---------- Public API ---------- */

bool Consolizer_Init(void)
{
	if(gecko_probe(EXI_SLOT_B))
	{
		gecko_channel = EXI_SLOT_B;
		gprintf("Consolizer: USB Gecko found on Slot B\n");
		return true;
	}
	if(gecko_probe(EXI_SLOT_A))
	{
		gecko_channel = EXI_SLOT_A;
		gprintf("Consolizer: USB Gecko found on Slot A\n");
		return true;
	}
	gprintf("Consolizer: USB Gecko not found!\n");
	return false;
}

bool Consolizer_Hello(void)
{
	u8 type = HELLO_TYPE_WIIFLOW;
	return gecko_send_frame(CMD_HELLO, &type, 1);
}

#define CONSOLIZER_RETRY_INTERVAL_MS  200
#define CONSOLIZER_RETRY_TOTAL_MS    10000
#define CONSOLIZER_MAX_ATTEMPTS      (CONSOLIZER_RETRY_TOTAL_MS / CONSOLIZER_RETRY_INTERVAL_MS)

/** Try a single HELLO + REQUEST_LIST + receive cycle. Returns game count or -1. */
static int _try_request_list(ConsolizerGameEntry *entries, int max_entries)
{
	/* Send HELLO */
	u8 type = HELLO_TYPE_WIIFLOW;
	if(!gecko_send_frame(CMD_HELLO, &type, 1))
		return -1;

	/* Send REQUEST_LIST */
	if(!gecko_send_frame(CMD_REQUEST_LIST, NULL, 0))
		return -1;

	int count = 0;
	u8 buf[128];
	u16 len = 0;

	while(count < max_entries)
	{
		u8 cmd = gecko_recv_frame(buf, sizeof(buf), &len);
		if(cmd == CMD_LIST_END)
			break;
		if(cmd != CMD_GAME_ENTRY || len < 11)
			return -1;  /* timeout or bad frame — Consolizer not ready */
		/* Parse: 6 bytes game_id + 4 bytes boot_token BE + 1 byte type */
		memcpy(entries[count].game_id, buf, 6);
		entries[count].game_id[6] = '\0';
		entries[count].boot_token =
			((u32)buf[6] << 24) | ((u32)buf[7] << 16) |
			((u32)buf[8] << 8)  |  (u32)buf[9];
		entries[count].type = buf[10];
		/* Optional title: 1-byte title_len + N-byte title */
		entries[count].title[0] = '\0';
		if(len >= 12)
		{
			u8 title_len = buf[11];
			if(title_len > CONSOLIZER_TITLE_MAX)
				title_len = CONSOLIZER_TITLE_MAX;
			if(len >= 12 + title_len)
			{
				memcpy(entries[count].title, &buf[12], title_len);
				entries[count].title[title_len] = '\0';
			}
		}
		count++;
	}

	return count;
}

int Consolizer_RequestGameList(ConsolizerGameEntry *entries, int max_entries)
{
	gprintf("Consolizer: waiting for bridge (up to %d sec)...\n",
	        CONSOLIZER_RETRY_TOTAL_MS / 1000);

	for(int attempt = 0; attempt < CONSOLIZER_MAX_ATTEMPTS; attempt++)
	{
		int count = _try_request_list(entries, max_entries);
		if(count >= 0)
		{
			gprintf("Consolizer: received %d games\n", count);
			return count;
		}
		if(attempt < CONSOLIZER_MAX_ATTEMPTS - 1)
		{
			gprintf("Consolizer: attempt %d/%d failed, retrying...\n",
			        attempt + 1, CONSOLIZER_MAX_ATTEMPTS);
			usleep(CONSOLIZER_RETRY_INTERVAL_MS * 1000);
		}
	}

	gprintf("Consolizer: bridge not responding after %d attempts\n",
	        CONSOLIZER_MAX_ATTEMPTS);
	return -1;
}

void Consolizer_SendLaunch(u32 boot_token)
{
	u8 payload[4];
	payload[0] = (u8)(boot_token >> 24);
	payload[1] = (u8)(boot_token >> 16);
	payload[2] = (u8)(boot_token >> 8);
	payload[3] = (u8)(boot_token & 0xFF);
	gecko_send_frame(CMD_LAUNCH, payload, 4);
}

u32 Consolizer_RequestSound(u32 boot_token, u8 **out_buf)
{
	*out_buf = NULL;
	if(gecko_channel < 0) return 0;

	/* Send REQUEST_SOUND [4 bytes: boot_token BE] */
	u8 payload[4];
	payload[0] = (u8)(boot_token >> 24);
	payload[1] = (u8)(boot_token >> 16);
	payload[2] = (u8)(boot_token >> 8);
	payload[3] = (u8)(boot_token & 0xFF);
	if(!gecko_send_frame(CMD_REQUEST_SOUND, payload, 4))
	{
		gprintf("Consolizer: failed to send REQUEST_SOUND\n");
		return 0;
	}

	/* Receive SOUND_CHUNK frames until SOUND_END (or SOUND_NONE).
	 * Use a modest stack buffer — sound thread only has 64KB stack. */
	u32 total = 0;
	u32 capacity = 0;
	u8 *buf = NULL;
	u8 chunk[8192];
	u16 len = 0;

	for(;;)
	{
		u8 cmd = gecko_recv_frame(chunk, sizeof(chunk), &len);
		if(cmd == CMD_SOUND_NONE || cmd == 0)
		{
			/* No sound or error */
			if(buf) free(buf);
			gprintf("Consolizer: sound not available (cmd=0x%02X)\n", cmd);
			return 0;
		}
		if(cmd == CMD_SOUND_END)
			break;
		if(cmd != CMD_SOUND_CHUNK)
		{
			/* Unexpected frame */
			if(buf) free(buf);
			gprintf("Consolizer: unexpected frame 0x%02X during sound transfer\n", cmd);
			return 0;
		}
		/* Grow buffer as needed */
		if(total + len > capacity)
		{
			u32 new_cap = (total + len + 0xFFFF) & ~0xFFFF; /* round up to 64K */
			u8 *new_buf = (u8 *)realloc(buf, new_cap);
			if(!new_buf)
			{
				if(buf) free(buf);
				gprintf("Consolizer: out of memory receiving sound (%u bytes)\n", total + len);
				return 0;
			}
			buf = new_buf;
			capacity = new_cap;
		}
		memcpy(buf + total, chunk, len);
		total += len;
	}

	if(total == 0 || buf == NULL)
	{
		if(buf) free(buf);
		return 0;
	}

	gprintf("Consolizer: received sound.bin (%u bytes)\n", total);
	*out_buf = buf;
	return total;
}
