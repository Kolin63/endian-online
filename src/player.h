#ifndef ENDIAN_PLAYER_H_
#define ENDIAN_PLAYER_H_

#if 1
// this is so this is included first and clang format doesnt move it
#include <concord/discord.h>
#endif

#include <concord/discord-response.h>

struct player {
  char* username;
  char* avatar;
  unsigned long uuid;
};

// initializes player
// returns pointer to player in registry
struct player* player_init(unsigned long uuid);

int player_cmp(const void* a, const void* b);

void player_cleanup(void* elem);

#endif
