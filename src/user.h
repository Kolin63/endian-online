#ifndef ENDIAN_USER_H_
#define ENDIAN_USER_H_

#include <concord/discord.h>
#include <concord/discord-response.h>

#define UUID_STR_LEN 32

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

int user_cmp(struct user* const* a, struct user* const* b);

void user_cleanup(struct user** elem);

// converts uuid (unsigned long) to string.
// string should be of length UUID_STR_LEN
void uuid_to_string(unsigned long uuid, char* buf);

// converts string to uuid (unsigned long)
unsigned long string_to_uuid(const char* str);

#endif

#endif
