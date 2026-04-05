#include "regman.h"

#include <concord/discord_codecs.h>
#include <stdlib.h>

#include "registry.h"

struct regman* global;

void regman_init() {
  global = malloc(sizeof(struct regman));

  global->command_registry =
      registry_init(sizeof(struct discord_create_global_application_command));
}

void regman_cleanup() {
  registry_cleanup(global->command_registry);
  free(global);
}

struct regman* regman_get() { return global; }

struct registry* regman_get_command() {
  return global->command_registry;
}
