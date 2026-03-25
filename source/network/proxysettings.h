/* Stub — proxy settings not used in Dolphin Consolizer build */
#ifndef _PROXYSETTINGS_H_
#define _PROXYSETTINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

static inline int useProxySettings(void) { return 0; }
static inline const char *getProxyAddress(void) { return ""; }
static inline int getProxyPort(void) { return 0; }
static inline const char *getProxyUsername(void) { return ""; }
static inline const char *getProxyPassword(void) { return ""; }

#ifdef __cplusplus
}
#endif

#endif /* _PROXYSETTINGS_H_ */
