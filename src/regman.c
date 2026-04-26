#include "regman.h"

#include <concord/discord_codecs.h>
#include <dlfcn.h>
#include <stdlib.h>

#include "command.h"
#include "function.h"
#include "player.h"
#include "plugin.h"
#include "registry.h"

struct regman* global;

void regman_init() {
  global = malloc(sizeof(struct regman));

  global->plugin_registry = registry_init(sizeof(struct plugin), plugin_cmp, plugin_cleanup);
  global->function_registry = registry_init(sizeof(struct function), function_cmp, function_cleanup);
  global->command_registry = registry_init(sizeof(struct command), command_cmp, command_cleanup);
  global->player_registry = registry_init(sizeof(struct player*), player_cmp, player_cleanup);
}

void regman_cleanup() {
  registry_cleanup(global->plugin_registry);
  registry_cleanup(global->function_registry);
  registry_cleanup(global->command_registry);
  registry_cleanup(global->player_registry);
  free(global);
}

struct regman* regman_get() { return global; }

struct registry* regman_get_plugin() { return global->plugin_registry; }
struct registry* regman_get_function() { return global->function_registry; }
struct registry* regman_get_command() { return global->command_registry; }
