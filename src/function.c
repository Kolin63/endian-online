#include "function.h"

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cJSON.h"
#include "fileio.h"
#include "json_iterator.h"
#include "json_macros.h"
#include "log.h"
#include "plugin.h"
#include "registry.h"
#include "regman.h"

// returns amount of errors, 0 if ok
int function_fillout(const char* mod_name, const char* file_name,
                     const cJSON* json, struct function* func) {
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
      if (strcmp(val, "CALLBACK") == 0) func->type = CALLBACK;
      else if (strcmp(val, "GET_API") == 0) func->type = GET_API;
      else if (strcmp(val, "EXPORT") == 0) func->type = EXPORT;
      else {
        log_error("In function %s from mod %s, unknown type %s", file_name, mod_name, val);
        error++;
        continue;
      }
    } else if (strcmp(item_name, "name") == 0) {
      END_JSON_CHECK_STRING;
      func->name = malloc(strlen(iter->json->valuestring) + 1);
      strcpy(func->name, iter->json->valuestring);
    } else if (strcmp(item_name, "plugin") == 0) {
      END_JSON_CHECK_STRING;
      func->plugin_name = malloc(strlen(iter->json->valuestring) + 1);
      strcpy(func->plugin_name, iter->json->valuestring);
    } else {
      log_error("Function %s from mod %s has unknown object %s", file_name, mod_name, iter->json->string);
      if (iter->json->type == cJSON_Array || iter->json->type == cJSON_Object)
        iter = json_iterator_skip_object(iter);
      error++;
      continue;
    }
  }

  json_iterator_cleanup(iter);
  return error;
}

void function_load(const char* function_path, const char* mod_name, const char* file_name) {
  if (strcmp(file_name, "template.json") == 0) return;

  FILE* file = fopen(function_path, "r");
  char* fileio = NULL;
  fileio_read_all(&fileio, file);

  cJSON* json = cJSON_Parse(fileio);

  free(fileio);
  fclose(file);

  struct function func = {};
  if (function_fillout(mod_name, file_name, json, &func) != 0) {
    cJSON_Delete(json);
    return;
  }

  cJSON_Delete(json);

  const struct plugin* plugin = registry_ktov(regman_get_plugin(), &(struct plugin){.name = func.plugin_name});
  if (plugin == NULL) {
    log_error("Could not find plugin %s while loading function %s from mod %s",
              func.plugin_name, func.name, mod_name);
    return;
  }

  dlerror();  // clear error
  void* handle = dlsym(plugin->plugin, func.name);

  const char* error = dlerror();
  if (error != NULL) {
    log_error("Error loading function %s from plugin %s from mod %s: %s",
              func.name, func.plugin_name, mod_name, error);
    return;
  }

  func.function = handle;

  if (registry_add(regman_get_function(), &func) == NULL) {
    log_error("Function %s already registered", func.name);
    return;
  }

  log_info("Loading function %s from plugin %s from mod %s", func.name,
           func.plugin_name, mod_name);
}

int function_cmp(const void* a, const void* b) {
  const struct function* x = a;
  const struct function* y = b;
  return registry_strcmp(x->name, y->name);
}

void function_cleanup(void* elem) {
  struct function* func = elem;
  free(func->name);
  free(func->plugin_name);
}
