#include "mod_loader.h"

#include <concord/discord.h>
#include <dirent.h>
#include <string.h>

#include "bot.h"
#include "command.h"
#include "log.h"

#define DIRECTORY_LOAD(path, func)                                   \
  {                                                                  \
    char fullpath[512];                                              \
    strcpy(fullpath, bot_get_global()->instance_dir);                \
    strcat(fullpath, "/mods/");                                      \
    strcat(fullpath, mod_name);                                      \
    strcat(fullpath, "/" path);                                      \
                                                                     \
    DIR* dir = opendir(fullpath);                                    \
    struct dirent* dirent;                                           \
                                                                     \
    if (!dir) {                                                      \
      log_error("Could not open folder from mod %s at %s", mod_name, \
                fullpath);                                           \
      return;                                                        \
    }                                                                \
                                                                     \
    while ((dirent = readdir(dir)) != NULL) {                        \
      if (dirent->d_name[0] == '.') {                                \
        continue;                                                    \
      }                                                              \
      const char* file_name = dirent->d_name;                        \
      func;                                                          \
    }                                                                \
    closedir(dir);                                                   \
  }

void commands_load(const struct discord_ready* event, const char* mod_name) {
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
    command_load(event, mod_name, dirent->d_name);
  }
  closedir(dir);
}

void data_load(const struct discord_ready* event, const char* mod_name) {
  DIRECTORY_LOAD("data/commands", command_load(event, mod_name, file_name));
}

void mod_load(const struct discord_ready* event, const char* mod_name) {
  log_info("Loading mod %s", mod_name);
  data_load(event, mod_name);
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
    mod_load(event, dirent->d_name);
  }
  closedir(dir);
}
