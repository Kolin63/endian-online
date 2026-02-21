#include <stdlib.h>

#include "bot.h"
#include "cli_args.h"

int main(int argc, char** argv) {
  struct cli_args* cli_args = malloc(sizeof(struct cli_args));
  cli_args_parse(argc, argv, cli_args);

  struct bot* bot = malloc(sizeof(struct bot));
  bot_init(bot, cli_args);
  bot_start(bot);

  free(bot);
  free(cli_args);

  return 0;
}
