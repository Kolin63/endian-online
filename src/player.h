#ifndef ENDIAN_PLAYER_H_
#define ENDIAN_PLAYER_H_

#include <concord/discord.h>
#include <concord/discord-response.h>

struct player {
  char* username;
  char* avatar;
  unsigned long uuid;
};

// initializes player
// returns pointer to player in registry
struct player* player_init(unsigned long uuid);

// returns pointer to player in registry
// initializes the player if it is not in registry
struct player* player_get(unsigned long uuid);

int player_cmp(const void* a, const void* b);

void player_cleanup(void* elem);

#endif
