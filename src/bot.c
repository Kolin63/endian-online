#include "bot.h"

#include <concord/discord.h>
#include <concord/logmod.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cli_args.h"
#include "log.h"
#include "mod_loader.h"
#include "sds.h"

static struct bot* global_bot = NULL;
static FILE* log_file_end = NULL;
static FILE* log_file_concord = NULL;

bool should_exit = false;

void on_ready(struct discord*, const struct discord_ready* event) {
  mod_loader_load_mods(event);
}

void on_interaction(struct discord*, const struct discord_interaction* event) {
  if (event->type != DISCORD_INTERACTION_APPLICATION_COMMAND)
    return; /* return if interaction isn't a slash command */

  if (strcmp(event->data->name, "ping") == 0) {
    struct discord_interaction_response params = {
        .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
        .data = &(struct discord_interaction_callback_data){.content = "pong"}};
    discord_create_interaction_response(global_bot->discord_bot, event->id,
                                        event->token, &params, NULL);
  }
  if (strcmp(event->data->name, "stop") == 0) {
    struct discord_interaction_response params = {
        .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
        .data = &(struct discord_interaction_callback_data){.content =
                                                                "Stopping..."}};
    discord_create_interaction_response(global_bot->discord_bot, event->id,
                                        event->token, &params, NULL);
    should_exit = true;
  }
}

void bot_init(struct cli_args* cli_args) {
  if (global_bot != NULL) {
    log_error("Global Bot already initialized");
    return;
  }
  global_bot = malloc(sizeof(struct bot));
  global_bot->instance_dir = cli_args->instance_dir;

  char token[128];
  sds token_file_path = sdsnew(global_bot->instance_dir);
  token_file_path = sdscat(token_file_path, "/token.txt");

  FILE* token_file = fopen(token_file_path, "r");

  if (!token_file) {
    printf("Could not open token file: %s", token_file_path);
    exit(EXIT_FAILURE);
  }

  fgets(token, sizeof(token), token_file);
  fclose(token_file);

  if (strlen(token) == 0) {
    printf("Bot token is of length 0");
    exit(EXIT_FAILURE);
  }

  // a new line in the token breaks concord
  if (token[strlen(token) - 1] == '\n') {
    token[strlen(token) - 1] = '\0';
  }

  global_bot->discord_bot = discord_init(token);

  sdsfree(token_file_path);

  // open logging files
  char log_time_name[128];

  time_t rawtime;
  struct tm* tm_time;
  time(&rawtime);
  tm_time = localtime(&rawtime);
  strftime(log_time_name, sizeof(log_time_name), "/logs/%Y-%m-%d-%H:%M:%S",
           tm_time);

  sds log_end_path = sdsnew(global_bot->instance_dir);
  log_end_path = sdscat(log_end_path, log_time_name);
  log_end_path = sdscat(log_end_path, ".end.log");

  sds log_concord_path = sdsnew(global_bot->instance_dir);
  log_concord_path = sdscat(log_concord_path, log_time_name);
  log_concord_path = sdscat(log_concord_path, ".concord.log");

  log_file_end = fopen(log_end_path, "w");
  log_file_concord = fopen(log_concord_path, "w");

  // setup logging from endian
  log_add_fp(log_file_end, LOG_TRACE);

  // setup logging from concord
  struct logmod_logger* logger =
      logmod_get_logger(discord_get_logmod(global_bot->discord_bot), "CLIENT");

  logmod_logger_set_logfile(logger, log_file_concord);
  logmod_logger_set_quiet(logger, 0);
#ifndef LOG_NO_USE_COLOR
  logmod_logger_set_color(logger, 1);
#endif

  if (cli_args->verbose == 0) {
    logmod_logger_set_quiet(
        logmod_get_logger(discord_get_logmod(global_bot->discord_bot),
                          "WEBSOCKETS"),
        1);
    logmod_logger_set_quiet(
        logmod_get_logger(discord_get_logmod(global_bot->discord_bot),
                          "WEBSOCKETS_RAW"),
        1);
    logmod_logger_set_quiet(
        logmod_get_logger(discord_get_logmod(global_bot->discord_bot), "HTTP"),
        1);
    logmod_logger_set_quiet(
        logmod_get_logger(discord_get_logmod(global_bot->discord_bot),
                          "HTTP_RAW"),
        1);
    logmod_logger_set_quiet(
        logmod_get_logger(discord_get_logmod(global_bot->discord_bot),
                          "GATEWAY"),
        1);
    logmod_logger_set_quiet(
        logmod_get_logger(discord_get_logmod(global_bot->discord_bot),
                          "REQUEST"),
        1);
    logmod_logger_set_quiet(
        logmod_get_logger(discord_get_logmod(global_bot->discord_bot),
                          "RATELIMIT"),
        1);
    logmod_logger_set_quiet(
        logmod_get_logger(discord_get_logmod(global_bot->discord_bot),
                          "REFCOUNT"),
        1);
  }

  sdsfree(log_end_path);
  sdsfree(log_concord_path);

  discord_set_on_ready(global_bot->discord_bot, &on_ready);
  discord_set_on_interaction_create(global_bot->discord_bot, &on_interaction);
}

void bot_cleanup() {
  discord_shutdown(global_bot->discord_bot);
  fclose(log_file_end);
  fclose(log_file_concord);
  free(global_bot);
}

struct bot* bot_get_global() { return global_bot; }

void bot_start() { discord_run(global_bot->discord_bot); }

bool bot_should_exit() { return should_exit; }
