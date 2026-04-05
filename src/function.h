#ifndef ENDIAN_FUNCTION_H_
#define ENDIAN_FUNCTION_H_

#include <concord/discord.h>

struct function {
  const void (*function)(struct discord* client,
                         const struct discord_interaction* event);
  char* name;
  char* plugin_name;
};

void function_load(const char* function_path, const char* mod_name,
                   const char* file_name);

#endif
