#ifndef ENDIAN_USER_H_
#define ENDIAN_USER_H_

#include <concord/discord.h>
#include <concord/discord-response.h>

struct user {
  char* username;
  char* avatar;
  unsigned long uuid;
};

#ifdef ENDIAN_ENGINE

// initializes user
// returns pointer to user in registry
struct user* user_init(unsigned long uuid);

// returns pointer to user in registry
// initializes the user if it is not in registry
struct user* user_get(unsigned long uuid);

int user_cmp(const void* a, const void* b);

void user_cleanup(void* elem);

#endif

#endif
