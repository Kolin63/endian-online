#ifndef ENDIAN_BOT_H_
#define ENDIAN_BOT_H_

#include <concord/discord.h>

#include "cli_args.h"

struct bot {
  char* instance_dir;

  struct discord* discord_bot;
};

void bot_init(struct bot* bot, struct cli_args* cli_args);

struct bot* bot_get_global();
void bot_start(struct bot* bot);

#endif
