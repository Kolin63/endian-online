#include <concord/discord.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>

#include "bot.h"
#include "cli_args.h"
#include "log.h"
#include "regman.h"

static bool should_exit = false;

static struct cli_args* cli_args;
static struct bot* bot;

void* cleanup(void*) {
  while (!should_exit && !bot_should_exit()) {
    ;
  }

  log_info("Stopping...");

  cli_args_cleanup(cli_args);
  regman_cleanup();
  bot_cleanup();
  printf("Done\n");

  exit(EXIT_SUCCESS);
}

void handle_exit() { should_exit = 1; }
void handle_sigint(int) { handle_exit(); }

int main(int argc, const char** argv) {
  regman_init();
  cli_args = cli_args_init();
  bot = malloc(sizeof(struct bot));

  pthread_t exit_thread;
  pthread_create(&exit_thread, NULL, cleanup, NULL);

  atexit(handle_exit);
  struct sigaction sa = {0};
  sa.sa_handler = handle_sigint;
  sigaction(SIGINT, &sa, NULL);

  cli_args_parse(argc, argv, cli_args);

  bot_init(cli_args);
  bot_start();

  // this line should never execute, but it's here just in case
  pthread_join(exit_thread, NULL);
  printf("endian exited in a weird way\n");

  return 0;
}
