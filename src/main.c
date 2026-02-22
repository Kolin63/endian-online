#include <concord/discord.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>

#include "bot.h"
#include "cli_args.h"
#include "log.h"

static bool should_exit = 0;

static struct cli_args* cli_args;
static struct bot* bot;

void* cleanup(void*) {
  while (!should_exit) {
    ;
  }

  log_info("Stopping...");

  discord_shutdown(bot_get_global()->discord_bot);

  free(cli_args);
  free(bot);
  printf("Done\n");

  exit(EXIT_SUCCESS);
  pthread_exit(NULL);
}

void handle_exit() { should_exit = 1; }
void handle_sigint(int) { handle_exit(); }

int main(int argc, char** argv) {
  pthread_t exit_thread;
  pthread_create(&exit_thread, NULL, cleanup, NULL);

  cli_args = malloc(sizeof(struct cli_args));
  bot = malloc(sizeof(struct bot));

  atexit(handle_exit);
  struct sigaction sa;
  sa.sa_handler = handle_sigint;
  sigaction(SIGINT, &sa, NULL);

  cli_args_parse(argc, argv, cli_args);

  bot_init(bot, cli_args);
  bot_start(bot);

  // this line should never execute, but it's here just in case
  pthread_join(exit_thread, NULL);
  printf("endian exited in a weird way\n");

  return 0;
}
