#include "mod_loader.h"

#include <concord/discord.h>
#include <concord/discord_codecs.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "bot.h"
#include "fileio.h"
#include "log.h"

void mod_loader_load_command(const struct discord_ready* event, char* mod_name,
                             char* cmd_name) {
  char path[512];
  strcpy(path, bot_get_global()->instance_dir);
  strcat(path, "/mods/");
  strcat(path, mod_name);
  strcat(path, "/data/commands/");
  strcat(path, cmd_name);

  struct fileio fileio = {.buf = malloc(1), .buf_size = 1};
  fileio.buf[0] = '\0';
  FILE* file = fopen(path, "r");

  if (!file) {
    log_error("Could not open command file from %s at %s", mod_name, path);
    return;
  }

  fileio_read_all(&fileio, file);

  struct discord_create_global_application_command params = {};
  discord_create_global_application_command_from_json(fileio.buf,
                                                      fileio.buf_size, &params);

  free(fileio.buf);

  discord_create_global_application_command(
      bot_get_global()->discord_bot, event->application->id, &params, NULL);
  log_info("Loading command %s from mod %s", params.name, mod_name);
}

void mod_loader_load_commands(const struct discord_ready* event,
                              char* mod_name) {
  char path[256];
  strcpy(path, bot_get_global()->instance_dir);
  strcat(path, "/mods/");
  strcat(path, mod_name);
  strcat(path, "/data/commands");

  DIR* dir = opendir(path);
  struct dirent* dirent;

  if (!dir) {
    log_error("Could not open commands folder from mod %s at %s", mod_name,
              path);
    return;
  }

  while ((dirent = readdir(dir)) != NULL) {
    if (dirent->d_name[0] == '.') {
      continue;
    }
    mod_loader_load_command(event, mod_name, dirent->d_name);
  }
}

void mod_loader_load_data(const struct discord_ready* event, char* mod_name) {
  mod_loader_load_commands(event, mod_name);
}

void mod_loader_load_mod(const struct discord_ready* event, char* mod_name) {
  log_info("Loading mod %s", mod_name);
  mod_loader_load_data(event, mod_name);
}

void mod_loader_load_mods(const struct discord_ready* event) {
  log_info("Loading mods!");

  char path[256];
  strcpy(path, bot_get_global()->instance_dir);
  strcat(path, "/mods");

  DIR* dir = opendir(path);
  struct dirent* dirent;

  if (!dir) {
    log_error("Could not open mods folder at %s", path);
    return;
  }

  while ((dirent = readdir(dir)) != NULL) {
    if (dirent->d_name[0] == '.') {
      continue;
    }
    mod_loader_load_mod(event, dirent->d_name);
  }
}
