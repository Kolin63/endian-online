#ifndef ENDIAN_COMMAND_H_
#define ENDIAN_COMMAND_H_

#include <concord/discord.h>
#include <concord/discord_codecs.h>

struct command {
  enum discord_application_command_types type;
  char* name;
  char* description;
  char* callback;
  struct discord_application_command_options* options;
  struct discord_application_command* dcmd;
  unsigned long default_member_permissions;
};

void command_load(const struct discord_ready* event, const char* command_path,
                  const char* mod_name, const char* file_name);

#endif
