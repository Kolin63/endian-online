#ifndef ENDIAN_COMMAND_H_
#define ENDIAN_COMMAND_H_

#include <concord/discord.h>
#include <concord/discord_codecs.h>

struct command {
  enum discord_application_command_types type;
  char name[33];
  char description[101];
  struct discord_application_command_options* options;
  unsigned long default_member_permissions;
};

void command_load(const struct discord_ready* event, const char* command_path,
                  const char* mod_name, const char* file_name);

#endif
