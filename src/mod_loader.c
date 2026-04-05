#include "mod_loader.h"

#include <concord/discord.h>
#include <dirent.h>

#include "bot.h"
#include "command.h"
#include "function.h"
#include "log.h"
#include "plugin.h"
#include "sds.h"

#define DIRECTORY_LOAD(pre_path, path, func)                         \
  {                                                                  \
    sds fullpath = sdsnew(pre_path);                                 \
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
      sds file_path = sdsnew(fullpath);                              \
      file_path = sdscat(file_path, "/");                            \
      file_path = sdscat(file_path, file_name);                      \
      func;                                                          \
      sdsfree(file_path);                                            \
    }                                                                \
    closedir(dir);                                                   \
    sdsfree(fullpath);                                               \
  }

void data_load(const struct discord_ready* event, const char* data_path,
               const char* mod_name) {
  DIRECTORY_LOAD(data_path, "functions",
                 function_load(file_path, mod_name, file_name));
  DIRECTORY_LOAD(data_path, "commands",
                 command_load(event, file_path, mod_name, file_name));
}

void mod_load(const struct discord_ready* event, const char* mod_path,
              const char* mod_name) {
  log_info("Loading mod %s", mod_name);

  DIRECTORY_LOAD(mod_path, "plugins",
                 plugin_load(file_path, mod_name, file_name));

  sds data_path = sdsnew(mod_path);
  data_path = sdscat(data_path, "/data");
  data_load(event, data_path, mod_name);
  sdsfree(data_path);
}

void mod_loader_load_mods(const struct discord_ready* event) {
  log_info("Loading mods!");

  const char* instance_dir = bot_get_global()->instance_dir;
  sds mods_path = sdsnew(instance_dir);
  mods_path = sdscat(mods_path, "/mods");

  // this is for logging within the macro
  const char* mod_name = "ENDIAN_ENGINE";

  DIRECTORY_LOAD(instance_dir, "mods", mod_load(event, file_path, file_name));

  sdsfree(mods_path);
}
