#include "command.h"

#include <concord/discord.h>
#include <concord/discord_codecs.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "bot.h"
#include "cJSON.h"
#include "command.h"
#include "fileio.h"
#include "json_iterator.h"
#include "log.h"
#include "registry.h"
#include "registry_manager.h"

void command_strict_check_json(const cJSON* json, char* mod_name,
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

// returns amount of errors, 0 if ok
int command_fillout(char* mod_name, char* cmd_name, cJSON* json,
                    struct discord_create_global_application_command* params) {
  int error = 0;

  struct json_iterator* iter = json_iterator_init(json);

  while (true) {
    iter = json_iterate(iter);
    if (iter->parent == NULL) break;
    if (iter->json == NULL) continue;

    const char* item_name = iter->json->string;

    if (strcmp(item_name, "type") == 0) {
      if (iter->json->type != cJSON_String) {
        log_error("In command %s from mod %s, type must be string", cmd_name,
                  mod_name);
        error++;
        continue;
      }
      if (iter->json->valuestring == NULL) {
        log_error("In command %s from mod %s, type is NULL", cmd_name,
                  mod_name);
        error++;
        continue;
      }
      const char* val = iter->json->valuestring;
      if (strcmp(val, "CHAT_INPUT") == 0)
        params->type = DISCORD_APPLICATION_CHAT_INPUT;
      else if (strcmp(val, "USER") == 0)
        params->type = DISCORD_APPLICATION_USER;
      else if (strcmp(val, "MESSAGE") == 0)
        params->type = DISCORD_APPLICATION_MESSAGE;
      else {
        log_error("In command %s from mod %s, unknown type %s", cmd_name,
                  mod_name, val);
        error++;
        continue;
      }
    } else if (strcmp(item_name, "name") == 0) {
      if (iter->json->type != cJSON_String) {
        log_error("In command %s from mod %s, name must be string", cmd_name,
                  mod_name);
        error++;
        continue;
      }
      if (iter->json->valuestring == NULL) {
        log_error("In command %s from mod %s, name is NULL", cmd_name,
                  mod_name);
        error++;
        continue;
      }
      int len = strlen(iter->json->valuestring);
      if (len < 1 || len > 32) {
        log_error("In command %s from mod %s, name is wrong size", cmd_name,
                  mod_name);
        error++;
        continue;
      }
      params->name = iter->json->valuestring;
    } else if (strcmp(item_name, "description") == 0) {
      if (iter->json->type != cJSON_String) {
        log_error("In command %s from mod %s, description must be string",
                  cmd_name, mod_name);
        error++;
        continue;
      }
      if (iter->json->valuestring == NULL) {
        log_error("In command %s from mod %s, description is NULL", cmd_name,
                  mod_name);
        error++;
        continue;
      }
      int len = strlen(iter->json->valuestring);
      if (len < 1 || len > 100) {
        log_error("In command %s from mod %s, description is wrong size",
                  cmd_name, mod_name);
        error++;
        continue;
      }
      params->description = iter->json->valuestring;
    } else if (strcmp(item_name, "options") == 0) {
      if (iter->json->type != cJSON_Array) {
        log_error("In command %s from mod %s, options must be array", cmd_name,
                  mod_name);
        error++;
        continue;
      }
      if (iter->json->child == NULL) {
        log_error("In command %s from mod %s, options has no items", cmd_name,
                  mod_name);
        error++;
        continue;
      }
      // TODO: load options
      iter = json_iterator_skip_object(iter);
    } else if (strcmp(item_name, "default_member_permissions") == 0) {
      if (iter->json->type != cJSON_String) {
        log_error(
            "In command %s from mod %s, default_member_permissions must be "
            "string",
            cmd_name, mod_name);
        error++;
        continue;
      }
      if (iter->json->valuestring == NULL) {
        log_error(
            "In command %s from mod %s, default_member_permissions is NULL",
            cmd_name, mod_name);
        error++;
        continue;
      }
      char* endptr = iter->json->valuestring;
      errno = 0;
      unsigned long long perms = strtoull(iter->json->valuestring, &endptr, 10);
      if (errno != 0 || *endptr != '\0') {
        log_error(
            "In command %s from mod %s, error reading "
            "default_member_permissions",
            cmd_name, mod_name);
      }
      params->default_member_permissions = perms;
    }
  }
  return error;
}

void command_load(const struct discord_ready* event, char* mod_name,
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

  command_strict_check_json(json, mod_name, cmd_name);

  struct discord_create_global_application_command params;
  if (command_fillout(mod_name, cmd_name, json, &params) != 0) return;

  registry_add(registry_manager_get_command_registry(), params.name,
               (void*)&params);
  discord_create_global_application_command(
      bot_get_global()->discord_bot, event->application->id, &params, NULL);
  log_info("Loading command %s from mod %s", cmd_name, mod_name);
}

void command_load_from_mod(const struct discord_ready* event, char* mod_name) {
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
