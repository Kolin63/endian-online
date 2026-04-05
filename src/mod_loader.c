#include "mod_loader.h"

#include <concord/discord.h>
#include <dirent.h>

#include "bot.h"
#include "command.h"
#include "log.h"
#include "sds.h"

#define DIRECTORY_LOAD(path, func)                                   \
  {                                                                  \
    sds fullpath = sdsnew(bot_get_global()->instance_dir);           \
    fullpath = sdscat(fullpath, "/mods/");                           \
    fullpath = sdscat(fullpath, mod_name);                           \
    fullpath = sdscat(fullpath, "/" path);                           \
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
    sdsfree(fullpath);                                               \
  }

void commands_load(const struct discord_ready* event, const char* mod_name) {
  sds path = sdsnew(bot_get_global()->instance_dir);
  path = sdscat(path, "/mods/");
  path = sdscat(path, mod_name);
  path = sdscat(path, "/data/commands");

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
  sdsfree(path);
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

  sds path = sdsnew(bot_get_global()->instance_dir);
  path = sdscat(path, "/mods");

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
  sdsfree(path);
}
