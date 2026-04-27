#include "regman.h"

#include <concord/discord_codecs.h>
#include <dlfcn.h>
#include <stdlib.h>

#include "command.h"
#include "function.h"
#include "user.h"
#include "plugin.h"
#include "registry.h"

struct regman* global;

void regman_init() {
  global = malloc(sizeof(struct regman));

  global->plugin = registry_init(sizeof(struct plugin), plugin_cmp, plugin_cleanup);
  global->function = registry_init(sizeof(struct function), function_cmp, function_cleanup);
  global->command = registry_init(sizeof(struct command), command_cmp, command_cleanup);
  global->user = registry_init(sizeof(struct user*), user_cmp, user_cleanup);
}

void regman_cleanup() {
  registry_cleanup(global->plugin);
  registry_cleanup(global->function);
  registry_cleanup(global->command);
  registry_cleanup(global->user);
  free(global);
}

struct regman* regman_get() { return global; }

struct registry* regman_get_plugin() { return global->plugin; }
struct registry* regman_get_function() { return global->function; }
struct registry* regman_get_command() { return global->command; }
