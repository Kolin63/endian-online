#include "bot.h"

#include <concord/discord.h>
#include <concord/logmod.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cli_args.h"

void on_ready(struct discord* client, const struct discord_ready* event) {
  struct discord_create_global_application_command params = {
      .name = "ping", .description = "Ping command!"};
  discord_create_global_application_command(client, event->application->id,
                                            &params, NULL);
}

void on_interaction(struct discord* client,
                    const struct discord_interaction* event) {
  if (event->type != DISCORD_INTERACTION_APPLICATION_COMMAND)
    return; /* return if interaction isn't a slash command */

  if (strcmp(event->data->name, "ping") == 0) {
    struct discord_interaction_response params = {
        .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
        .data = &(struct discord_interaction_callback_data){.content = "pong"}};
    discord_create_interaction_response(client, event->id, event->token,
                                        &params, NULL);
  }
}

void bot_init(struct bot* bot, struct cli_args* cli_args) {
  bot->instance_dir = cli_args->instance_dir;

  if (cli_args->run_discord) {
    char token[128];
    char token_file_path[128];
    strcpy(token_file_path, bot->instance_dir);
    strcat(token_file_path, "/token.txt");

    FILE* token_file = fopen(token_file_path, "r");

    if (!token_file) {
      printf("Could not open token file: %s", token_file_path);
      exit(EXIT_FAILURE);
    }

    fgets(token, sizeof(token), token_file);

    if (strlen(token) == 0) {
      printf("Bot token is of length 0");
      exit(EXIT_FAILURE);
    }

    // a new line in the token breaks concord
    if (token[strlen(token) - 1] == '\n') {
      token[strlen(token) - 1] = '\0';
    }

    bot->discord_bot = discord_init(token);

    char log_file_path[256];
    char log_time_name[64];
    time_t rawtime;
    struct tm* tm_time;
    time(&rawtime);
    tm_time = localtime(&rawtime);
    strftime(log_time_name, sizeof(log_time_name), "/logs/%Y%m%e%H%M%S.log",
             tm_time);
    strcpy(log_file_path, bot->instance_dir);
    strcat(log_file_path, log_time_name);

    struct logmod_logger* logger =
        logmod_get_logger(discord_get_logmod(bot->discord_bot), "CLIENT");

    FILE* file = fopen(log_file_path, "w");
    logmod_logger_set_logfile(logger, file);
    logmod_logger_set_quiet(logger, 0);
    logmod_logger_set_color(logger, 1);

    if (cli_args->verbose == 0) {
      logmod_logger_set_quiet(
          logmod_get_logger(discord_get_logmod(bot->discord_bot), "WEBSOCKETS"),
          1);
      logmod_logger_set_quiet(
          logmod_get_logger(discord_get_logmod(bot->discord_bot),
                            "WEBSOCKETS_RAW"),
          1);
      logmod_logger_set_quiet(
          logmod_get_logger(discord_get_logmod(bot->discord_bot), "HTTP"), 1);
      logmod_logger_set_quiet(
          logmod_get_logger(discord_get_logmod(bot->discord_bot), "HTTP_RAW"),
          1);
      logmod_logger_set_quiet(
          logmod_get_logger(discord_get_logmod(bot->discord_bot), "GATEWAY"),
          1);
      logmod_logger_set_quiet(
          logmod_get_logger(discord_get_logmod(bot->discord_bot), "REQUEST"),
          1);
      logmod_logger_set_quiet(
          logmod_get_logger(discord_get_logmod(bot->discord_bot), "RATELIMIT"),
          1);

      logmod_log(INFO, logger, "Hello");
    }

    discord_set_on_ready(bot->discord_bot, &on_ready);
    discord_set_on_interaction_create(bot->discord_bot, &on_interaction);
    discord_run(bot->discord_bot);
  }
}
