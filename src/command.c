#include "command.h"

#include <concord/application_command.h>
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
#include "json_macros.h"
#include "log.h"
#include "registry.h"
#include "regman.h"

// returns amount of errors, 0 if ok
int command_fillout(const char* mod_name, const char* file_name,
                    const cJSON* json, struct command* params) {
  int error = 0;

  struct json_iterator* iter = json_iterator_init(json);

  while (true) {
    iter = json_iterate(iter);
    if (iter->parent == NULL) break;
    if (iter->json == NULL) continue;

    const char* item_name = iter->json->string;

    if (strcmp(item_name, "type") == 0) {
      END_JSON_CHECK_STRING;
      const char* val = iter->json->valuestring;
      if (strcmp(val, "CHAT_INPUT") == 0)
        params->type = DISCORD_APPLICATION_CHAT_INPUT;
      else if (strcmp(val, "USER") == 0)
        params->type = DISCORD_APPLICATION_USER;
      else if (strcmp(val, "MESSAGE") == 0)
        params->type = DISCORD_APPLICATION_MESSAGE;
      else {
        log_error("In command %s from mod %s, unknown type %s", file_name,
                  mod_name, val);
        error++;
        continue;
      }
    } else if (strcmp(item_name, "name") == 0) {
      END_JSON_CHECK_STRING;
      END_JSON_CHECK_STRING_LENGTH(1, 32);
      params->name = malloc(strlen(iter->json->valuestring));
      strcpy(params->name, iter->json->valuestring);
    } else if (strcmp(item_name, "description") == 0) {
      END_JSON_CHECK_STRING;
      END_JSON_CHECK_STRING_LENGTH(1, 100);
      params->description = malloc(strlen(iter->json->valuestring));
      strcpy(params->description, iter->json->valuestring);
    } else if (strcmp(item_name, "options") == 0) {
      END_JSON_CHECK_ARRAY;
      // TODO: load options
      iter = json_iterator_skip_object(iter);
    } else if (strcmp(item_name, "default_member_permissions") == 0) {
      END_JSON_CHECK_STRING;
      char* endptr = iter->json->valuestring;
      errno = 0;
      unsigned long perms = strtoul(iter->json->valuestring, &endptr, 10);
      if (errno != 0 || *endptr != '\0') {
        log_error(
            "In command %s from mod %s, error reading "
            "default_member_permissions",
            file_name, mod_name);
      }
      params->default_member_permissions = perms;
    } else if (strcmp(item_name, "callback") == 0) {
      END_JSON_CHECK_STRING;
      const char* func = iter->json->valuestring;
      if (registry_ktoi(regman_get_function(), func) == -1) {
        log_error("In command %s from mod %s, function %s not registered",
                  file_name, mod_name, func);
      }
      params->callback = malloc(strlen(func));
      strcpy(params->callback, func);
    } else {
      log_error("Command %s from mod %s has unknown object %s", file_name,
                mod_name, iter->json->string);
      if (iter->json->type == cJSON_Array || iter->json->type == cJSON_Object)
        json_iterator_skip_object(iter);
    }
  }

  json_iterator_cleanup(iter);
  return error;
}

void command_load(const struct discord_ready* event, const char* command_path,
                  const char* mod_name, const char* file_name) {
  if (strcmp(file_name, "template.json") == 0) return;

  struct fileio* fileio = fileio_init();
  FILE* file = fopen(command_path, "r");

  if (!file) {
    log_error("Could not open command file from %s at %s", mod_name,
              command_path);
    return;
  }

  fileio_read_all(fileio, file);

  cJSON* json = cJSON_Parse(fileio->buf);

  fileio_cleanup(fileio);
  fclose(file);

  struct command params = {};
  if (command_fillout(mod_name, file_name, json, &params) != 0) return;

  if (registry_add(regman_get_command(), params.name, (void*)&params) == -1) {
    log_error("Command %s already registered", file_name);
    return;
  };

  struct discord_create_global_application_command discord_params = {
      .type = params.type,
      .name = params.name,
      .description = params.description,
      .options = params.options,
      .default_member_permissions = params.default_member_permissions};

  discord_create_global_application_command(bot_get_global()->discord_bot,
                                            event->application->id,
                                            &discord_params, NULL);
  log_info("Loading command %s from mod %s", file_name, mod_name);

  cJSON_Delete(json);
}
