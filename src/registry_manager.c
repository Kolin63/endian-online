#include "registry_manager.h"

#include <concord/discord_codecs.h>
#include <stdlib.h>

#include "registry.h"

struct registry_manager* global;

void registry_manager_init() {
  global = malloc(sizeof(struct registry_manager));

  global->command_registry =
      registry_init(sizeof(struct discord_create_global_application_command));
}

void registry_manager_cleanup() {
  registry_cleanup(global->command_registry);
  free(global);
}

struct registry_manager* registry_manager_get_global() { return global; }

struct registry* registry_manager_get_command_registry() {
  return global->command_registry;
}
