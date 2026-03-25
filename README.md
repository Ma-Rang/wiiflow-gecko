# WiiFlow Lite — Dolphin Consolizer Fork

A modified version of [WiiFlow Lite](https://github.com/Fledge68/WiiFlow_Lite) adapted to run inside [Dolphin Emulator](https://dolphin-emu.org/) as a game browser for the **Dolphin Consolizer** project. This fork replaces WiiFlow's native USB/SD game scanning with a USB Gecko protocol that receives the game list from the Consolizer companion app.

## What This Fork Changes

This is **not** a general-purpose WiiFlow build. It is specifically designed to run inside Dolphin and communicate with the Dolphin Consolizer desktop app. Key modifications:

- **Consolizer integration** (`source/consolizer/`): USB Gecko protocol layer that receives the game list from the Consolizer app and sends launch commands back when the user selects a game.
- **IOS patching disabled** (`source/channel/nand.cpp`): `PatchIOS()` scans ARM memory regions that aren't mapped for PPC reads in Dolphin, causing crashes. Disabled since IOS patching is unnecessary in an emulator.
- **Gecko debug framing** (`source/gecko/gecko.cpp`): `gprintf` output is wrapped in framed packets so the Consolizer app can display WiiFlow's debug log.
- **SD card superfloppy mount** (`source/devicemounter/PartitionHandle.cpp`): Dolphin's virtual SD card is a raw FAT32 volume (no MBR). Added a fallback mount at sector 0 for superfloppy images.
- **Retry logic for Consolizer connection** (`source/consolizer/consolizer.cpp`): The Gecko TCP bridge may not be connected when WiiFlow first requests the game list. Retries with 200ms intervals for up to 10 seconds.
- **Error diagnostics** (`source/loader/main.cpp`): When initialization fails, displays a console error screen with SD card status instead of silently exiting.
- **Hardware features stripped**: USB storage, NTFS/ext support, and other hardware-dependent features that don't apply in Dolphin have been removed or stubbed out.
- **NAND title boot stub** (`source/loader/stub.S`): Custom `.stub` section at physical address 0x3400 that sets up BATs and enables the MMU before jumping to `_start`. Required because Dolphin starts NAND titles with the MMU off.

## USB Gecko Protocol

WiiFlow communicates with the Consolizer app via USB Gecko (EXI Slot B → TCP bridge in Dolphin):

| Direction | Cmd | ID | Payload |
|-----------|-----|----|---------|
| Wii → Consolizer | HELLO | `0x10` | 1 byte: `0x02` (identifies as WiiFlow) |
| Wii → Consolizer | REQUEST_LIST | `0x12` | — |
| Wii → Consolizer | LAUNCH | `0x13` | 4 bytes: boot token (BE) |
| Wii → Consolizer | REQUEST_SOUND | `0x14` | 4 bytes: boot token (BE) |
| Consolizer → Wii | GAME_ENTRY | `0x01` | 6-byte game ID + 4-byte boot token + 1-byte type + 1-byte title len + title |
| Consolizer → Wii | LIST_END | `0x02` | — |
| Consolizer → Wii | SOUND_CHUNK | `0x05` | Up to 65535 bytes of banner sound data |
| Consolizer → Wii | SOUND_END | `0x06` | — |
| Consolizer → Wii | SOUND_NONE | `0x07` | — |

## Building

Requires [devkitPPC](https://devkitpro.org/) (devkitPro toolchain for PowerPC/Wii).

```bash
# Set environment (devkitPro msys2 shell recommended)
export DEVKITPRO=/opt/devkitpro
export DEVKITPPC=/opt/devkitpro/devkitPPC
export PATH=$DEVKITPRO/tools/bin:$DEVKITPPC/bin:$PATH

make clean
make
```

Output: `out/boot.dol`

### Windows notes

- **Use devkitPro's msys2 bash** — the standard Git Bash / MSYS2 may not propagate `DEVKITPRO` environment variables to `make`.
- The build path must not contain spaces.
- `TMPDIR`/`TMP`/`TEMP` may need to be set to a writable directory if the default temp path is inaccessible.

## SD Card Setup

WiiFlow reads its themes, config, and cover art from Dolphin's virtual SD card:

- Enable the virtual SD card in Dolphin (Config > Wii > Insert SD Card)
- The SD image should be superfloppy format (FAT32, no MBR) — this fork handles that automatically
- Required: `sd:/apps/wiiflow/` with WiiFlow config and theme files
- Game list comes from the Consolizer app via USB Gecko — "No games found" on SD is expected

## Upstream

Based on [WiiFlow Lite](https://github.com/Fledge68/WiiFlow_Lite) by Fledge68 and contributors.

## License

GPL v3 (same as upstream WiiFlow Lite). See individual source file headers for details.
