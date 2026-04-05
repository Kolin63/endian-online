#ifndef ENDIAN_COMMAND_H_
#define ENDIAN_COMMAND_H_

#include <concord/discord.h>

void command_load(const struct discord_ready* event, char* mod_name,
                  char* cmd_name);

#endif
