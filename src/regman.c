#include "regman.h"

#include <concord/discord_codecs.h>
#include <dlfcn.h>
#include <stdlib.h>

#include "command.h"
#include "function.h"
#include "plugin.h"
#include "registry.h"

struct regman* global;

void regman_init() {
  global = malloc(sizeof(struct regman));

  global->plugin_registry = registry_init(sizeof(struct plugin));
  global->function_registry = registry_init(sizeof(struct function));
  global->command_registry = registry_init(sizeof(struct command));
}

void regman_cleanup() {
  for (int i = 0; i < global->plugin_registry->length; i++) {
    struct plugin* plugin = registry_itov(global->plugin_registry, i);
    dlclose(plugin->plugin);
  }
  registry_cleanup(global->plugin_registry);
  registry_cleanup(global->function_registry);
  registry_cleanup(global->command_registry);
  free(global);
}

struct regman* regman_get() { return global; }

struct registry* regman_get_plugin() { return global->plugin_registry; }

struct registry* regman_get_function() { return global->function_registry; }

struct registry* regman_get_command() { return global->command_registry; }
