#include "api.h"

#include <concord/interaction.h>
#include <stdlib.h>

#include "function.h"
#include "log.h"
#include "registry.h"
#include "regman.h"

static struct api* global;

void api_init() {
  global = malloc(sizeof(struct api));
  global->version = API_VERSION;
  global->log_log = log_log;
  global->discord_create_interaction_response =
      discord_create_interaction_response;
  global->registry_init = registry_init;
  global->registry_cleanup = registry_cleanup;
  global->registry_add = registry_add;
  global->registry_itov = registry_itov;
  global->registry_itov_safe = registry_itov_safe;
  global->registry_ktoi = registry_ktoi;
  global->registry_ktov = registry_ktov;
  global->get_plugin_registry = (void*)regman_get_plugin;
  global->get_function_registry = (void*)regman_get_function;
  global->get_command_registry = (void*)regman_get_command;
}

void api_cleanup() { free(global); }

const struct api* api_get_global() { return global; }

// hands out api to all mod functions of type GET_API
void api_distribute() {
  const struct registry* func_reg = regman_get_function();
  for (int i = 0; i < func_reg->length; i++) {
    const struct function* func = registry_itov(func_reg, i);
    if (func->type != GET_API) continue;
    func->function(global);
  }
}
