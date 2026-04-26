#include <concord/discord.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include "api.h"
#include "bot.h"
#include "cli_args.h"
#include "log.h"
#include "regman.h"

static struct cli_args* cli_args;
static pthread_once_t cleanup_once = PTHREAD_ONCE_INIT;
static volatile sig_atomic_t cleanup_ready = 0;

void cleanup() {
  log_info("Stopping...");
  if (bot_get_global() != NULL) {
    discord_shutdown(bot_get_global()->discord_bot);
  }
}

void handle_exit() {
  if (cleanup_ready == 0) {
    log_warn("Cannot exit yet");
    return;
  }
  pthread_once(&cleanup_once, cleanup);
}
void handle_sigint(int) { handle_exit(); }
void set_cleanup_ready() { cleanup_ready = 1; }

int main(int argc, const char** argv) {
  struct sigaction sa = {0};
  sa.sa_handler = handle_sigint;
  sigaction(SIGINT, &sa, NULL);

  regman_init();
  cli_args = cli_args_init();
  api_init();

  cli_args_parse(argc, argv, cli_args);

  bot_init(cli_args);
  bot_start();

  regman_cleanup();
  cli_args_cleanup(cli_args);
  api_cleanup();
  bot_cleanup();

  log_info("Done");

  return 0;
}
