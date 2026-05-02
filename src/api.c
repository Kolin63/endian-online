#include "api.h"

#include <concord/interaction.h>
#include <stdlib.h>

#include "exit.h"
#include "function.h"
#include "log.h"
#include "registry.h"
#include "regman.h"
#include "save.h"
#include "user.h"

// forward declaration. defined in main.c
void handle_exit();

static struct api* global;

void api_init() {
  global = malloc(sizeof(struct api));

  global->version = API_VERSION;

  global->log_log = log_log;

  global->exit = handle_exit;
  global->abort_cleanup = abort_cleanup;

  global->discord_create_interaction_response = discord_create_interaction_response;

  global->registry_init = registry_init;
  global->registry_cleanup = registry_cleanup;
  global->registry_add = registry_add;
  global->registry_itov = registry_itov;
  global->registry_itov_safe = registry_itov_safe;
  global->registry_ktoi = registry_ktoi;
  global->registry_ktov = registry_ktov;
  global->registry_strcmp = registry_strcmp;

  global->get_plugin_registry = (void*)regman_get_plugin;
  global->get_function_registry = (void*)regman_get_function;
  global->get_command_registry = (void*)regman_get_command;

  global->user_init = user_init;
  global->user_get = user_get;
  global->user_cmp = user_cmp;
  global->user_cleanup = user_cleanup;
  global->uuid_to_string = uuid_to_string;
  global->string_to_uuid = string_to_uuid;

  global->save_write = save_write;
  global->save_read = save_read;
}

void api_cleanup() { free(global); }

const struct api* api_get_global() { return global; }

void api_distribute() {
  const struct registry* func_reg = regman_get_function();
  for (int i = 0; i < func_reg->length; i++) {
    const struct function* func = registry_itov(func_reg, i);
    if (func->type != FT_GET_API) continue;
    func->function(global);
  }
}

// calls all mod functions of type LOAD
void api_call_load_func() {
  const struct registry* func_reg = regman_get_function();
  for (int i = 0; i < func_reg->length; i++) {
    const struct function* func = registry_itov(func_reg, i);
    if (func->type != FT_LOAD) continue;
    func->function(global);
  }
}
