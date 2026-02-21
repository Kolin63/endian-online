#include <stdlib.h>

#include "bot.h"
#include "cli_args.h"

int main(int argc, char** argv) {
  struct cli_args cli_args = {};
  cli_args_parse(argc, argv, &cli_args);

  struct bot* bot = malloc(sizeof(struct bot));
  bot_init(bot, &cli_args);

  free(bot);

  return 0;
}
