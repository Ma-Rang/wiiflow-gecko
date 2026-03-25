/* Stub — gamercard providers not used in Dolphin Consolizer build */
#ifndef _GCARD_H_
#define _GCARD_H_

#ifdef __cplusplus
extern "C" {
#endif

static inline void register_card_provider(const char *url, const char *key) { (void)url; (void)key; }
static inline int has_enabled_providers(void) { return 0; }
static inline void update_card(const char *id) { (void)id; }

#ifdef __cplusplus
}
#endif

#endif /* _GCARD_H_ */
