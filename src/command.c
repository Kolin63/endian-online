#include "command.h"

#include <concord/discord.h>
#include <dirent.h>
#include <string.h>

#include "bot.h"
#include "cJSON.h"
#include "command.h"
#include "fileio.h"
#include "json_iterator.h"
#include "log.h"
#include "registry.h"
#include "registry_manager.h"

void strict_check_command_json(const cJSON* json, char* mod_name,
                               char* cmd_name) {
  char template_path[512];
  strcpy(template_path, bot_get_global()->instance_dir);
  strcat(template_path, "/mods/endian-base/data/commands/template.json");

  struct fileio* fileio = fileio_init();
  FILE* template_file = fopen(template_path, "r");
  if (!template_file) {
    log_error("Could not open template command JSON file at %s", template_path);
  }
  fileio_read_all(fileio, template_file);

  cJSON* template = cJSON_Parse(fileio->buf);

  struct json_iterator* iter = json_iterator_init(json);

  // iterate once so that we get inside the root object, then we will skip all
  // other children so we don't get inside arrays or other objects
  iter = json_iterate(iter);

  // check each object in the command json, fail if it does not exist in the
  // template
  while (true) {
    iter = json_iterate(iter);
    if (iter->parent == NULL) break;
    if (iter->json == NULL) continue;

    if (!cJSON_HasObjectItem(template, iter->json->string)) {
      log_error("Command %s from mod %s has object %s which is not in template",
                cmd_name, mod_name, iter->json->string);
    }

    if (iter->json->type == cJSON_Array || iter->json->type == cJSON_Object)
      json_iterator_skip_object(iter);
  }

  json_iterator_cleanup(iter);
  cJSON_Delete(template);
}

void load_command(const struct discord_ready* event, char* mod_name,
                  char* cmd_name) {
  if (strcmp(cmd_name, "template.json") == 0) return;

  char path[512];
  strcpy(path, bot_get_global()->instance_dir);
  strcat(path, "/mods/");
  strcat(path, mod_name);
  strcat(path, "/data/commands/");
  strcat(path, cmd_name);

  struct fileio* fileio = fileio_init();
  FILE* file = fopen(path, "r");

  if (!file) {
    log_error("Could not open command file from %s at %s", mod_name, path);
    return;
  }

  fileio_read_all(fileio, file);

  cJSON* json = cJSON_Parse(fileio->buf);

  fileio_cleanup(fileio);
  fclose(file);

  strict_check_command_json(json, mod_name, cmd_name);

  // registry_add(registry_manager_get_command_registry(), params.name,
  //              (void*)&params);
  // discord_create_global_application_command(
  //     bot_get_global()->discord_bot, event->application->id, &params, NULL);
  log_info("Loading command %s from mod %s", cmd_name, mod_name);
}

void load_commands(const struct discord_ready* event, char* mod_name) {
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
    load_command(event, mod_name, dirent->d_name);
  }
  closedir(dir);
}
